﻿#include "ModelLoader.h"

ModelLoader::ModelLoader(QObject *parent) : QObject(parent)
{

}

bool ModelLoader::load(const QString path)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path.toStdString(),
            aiProcess_GenSmoothNormals |
            aiProcess_CalcTangentSpace |
            aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices |
            aiProcess_SortByPType
            );

    if( !scene)
    {
        qDebug() << "Error loading file: (assimp:) " << importer.GetErrorString();
        return false;
    }

    if(scene->HasMaterials())
    {
        for(unsigned int ii=0; ii<scene->mNumMaterials; ++ii)
        {
            QSharedPointer<MaterialInfo> mater = processMaterial(scene->mMaterials[ii]);
            m_materials.push_back(mater);
        }
    }

    if(scene->HasMeshes())
    {
        for(unsigned int ii=0; ii<scene->mNumMeshes; ++ii)
        {
            m_meshes.push_back(processMesh(scene->mMeshes[ii]));
        }
    }
    else
    {
        qDebug() << "Error: No meshes found";
        return false;
    }

    if(scene->HasLights())
    {
        qDebug() << "Has Lights";
    }

    if(scene->mRootNode != NULL)
    {
        Node *rootNode = new Node;
        processNode(scene, scene->mRootNode, 0, *rootNode);
        m_rootNode.reset(rootNode);
    }
    else
    {
        qDebug() << "Error loading model";
        return false;
    }

    // This will transform the model to unit coordinates, so a model of any size or shape will fit on screen
    if (m_transformToUnitCoordinates)
        transformToUnitCoordinates();

    return true;
}

void ModelLoader::getBufferData(QVector<float> **vertices, QVector<float> **normals, QVector<unsigned int> **indices)
{
    if(vertices != nullptr)
        *vertices = &m_vertices;

    if(normals != nullptr)
        *normals = &m_normals;

    if(indices != nullptr)
        *indices = &m_indices;
}

QSharedPointer<MaterialInfo> ModelLoader::processMaterial(aiMaterial *material)
{
    QSharedPointer<MaterialInfo> mater(new MaterialInfo);

    aiString mname;
    material->Get(AI_MATKEY_NAME, mname);
    if (mname.length > 0)
        mater->name = mname.C_Str();

    int shadingModel;
    material->Get(AI_MATKEY_SHADING_MODEL, shadingModel);

    if (shadingModel != aiShadingMode_Phong && shadingModel != aiShadingMode_Gouraud)
    {
        qDebug() << "This mesh's shading model is not implemented in this loader, setting to default material";
        mater->name = "DefaultMaterial";
    }
    else
    {
        aiColor3D dif(0.f,0.f,0.f);
        aiColor3D amb(0.f,0.f,0.f);
        aiColor3D spec(0.f,0.f,0.f);
        float shine = 0.0;

        material->Get(AI_MATKEY_COLOR_AMBIENT, amb);
        material->Get(AI_MATKEY_COLOR_DIFFUSE, dif);
        material->Get(AI_MATKEY_COLOR_SPECULAR, spec);
        material->Get(AI_MATKEY_SHININESS, shine);

        mater->ambient = QVector3D(amb.r, amb.g, amb.b);
        mater->diffuse = QVector3D(dif.r, dif.g, dif.b);
        mater->specular = QVector3D(spec.r, spec.g, spec.b);
        mater->shiness = shine;

        mater->ambient *= .2f;
        if (mater->shiness == 0.0f)
            mater->shiness = 30.f;
    }
    return mater;
}

QSharedPointer<Mesh> ModelLoader::processMesh(aiMesh *mesh)
{
    QSharedPointer<Mesh> newMesh(new Mesh);
    newMesh->name = mesh->mName.length != 0 ? mesh->mName.C_Str() : "";
    newMesh->indexOffset = m_indices.size();
    unsigned int indexCountBefore = m_indices.size();
    int vertindexoffset = m_vertices.size()/3;

    // Get Vertices
    if (mesh->mNumVertices > 0)
    {
        for (uint ii = 0; ii < mesh->mNumVertices; ++ii)
        {
            aiVector3D &vec = mesh->mVertices[ii];

            m_vertices.push_back(vec.x);
            m_vertices.push_back(vec.y);
            m_vertices.push_back(vec.z);
        }
    }

    // Get Normals
    if (mesh->HasNormals())
    {
        for (uint ii = 0; ii < mesh->mNumVertices; ++ii)
        {
            aiVector3D &vec = mesh->mNormals[ii];
            m_normals.push_back(vec.x);
            m_normals.push_back(vec.y);
            m_normals.push_back(vec.z);
        }
    }

    // Get mesh indexes
    for (uint t = 0; t < mesh->mNumFaces; ++t)
    {
        aiFace* face = &mesh->mFaces[t];
        if (face->mNumIndices != 3)
        {
            qDebug() << "Warning: Mesh face with not exactly 3 indices, ignoring this primitive.";
            continue;
        }

        m_indices.push_back(face->mIndices[0]+vertindexoffset);
        m_indices.push_back(face->mIndices[1]+vertindexoffset);
        m_indices.push_back(face->mIndices[2]+vertindexoffset);
    }

    newMesh->indexCount = m_indices.size() - indexCountBefore;
    newMesh->material = m_materials.at(mesh->mMaterialIndex);

    return newMesh;
}

void ModelLoader::processNode(const aiScene *scene, aiNode *node, Node *parentNode, Node &newNode)
{
    newNode.name = node->mName.length != 0 ? node->mName.C_Str() : "";

    newNode.transformation = QMatrix4x4(node->mTransformation[0]);

    newNode.meshes.resize(node->mNumMeshes);
    for (uint imesh = 0; imesh < node->mNumMeshes; ++imesh)
    {
        QSharedPointer<Mesh> mesh = m_meshes[node->mMeshes[imesh]];
        newNode.meshes[imesh] = mesh;
    }

    for (uint ich = 0; ich < node->mNumChildren; ++ich)
    {
        newNode.nodes.push_back(Node());
        processNode(scene, node->mChildren[ich], parentNode, newNode.nodes[ich]);
    }
}

void ModelLoader::transformToUnitCoordinates()
{
    // This will transform the model to unit coordinates, so a model of any size or shape will fit on screen

    double amin = std::numeric_limits<double>::max();
    double amax = std::numeric_limits<double>::min();
    QVector3D minDimension(amin,amin,amin);
    QVector3D maxDimension(amax,amax,amax);

    // Get the minimum and maximum x,y,z values for the model
    findObjectDimensions(m_rootNode.data(), QMatrix4x4(), minDimension, maxDimension);

    // Calculate scale and translation needed to center and fit on screen
    float dist = qMax(maxDimension.x() - minDimension.x(), qMax(maxDimension.y()-minDimension.y(), maxDimension.z() - minDimension.z()));
    float sc = 1.0/dist;
    QVector3D center = (maxDimension - minDimension)/2;
    QVector3D trans = -(maxDimension - center);

    // Apply the scale and translation to a matrix
    QMatrix4x4 transformation;
    transformation.setToIdentity();
    transformation.scale(sc);
    transformation.translate(trans);

    // Multiply the transformation to the root node transformation matrix
    m_rootNode.data()->transformation = transformation * m_rootNode.data()->transformation;
}

void ModelLoader::findObjectDimensions(Node *node, QMatrix4x4 transformation, QVector3D &minDimension, QVector3D &maxDimension)
{
    transformation *= node->transformation;

    for (int ii=0; ii<node->meshes.size(); ++ii) {
        int start = node->meshes[ii]->indexOffset;
        int end = start + node->meshes[ii]->indexCount;
        for(int ii=start; ii<end; ++ii)
        {
            int ind = m_indices[ii] * 3;
            QVector4D vec(m_vertices[ind], m_vertices[ind+1], m_vertices[ind+2], 1.0);
            vec = transformation * vec;

            if(vec.x() < minDimension.x())
                minDimension.setX(vec.x());
            if(vec.y() < minDimension.y())
                minDimension.setY(vec.y());
            if(vec.z() < minDimension.z())
                minDimension.setZ(vec.z());
            if(vec.x() > maxDimension.x())
                maxDimension.setX(vec.x());
            if(vec.y() > maxDimension.y())
                maxDimension.setY(vec.y());
            if(vec.z() > maxDimension.z())
                maxDimension.setZ(vec.z());
        }
    }

    for (int ii=0; ii<node->nodes.size(); ++ii) {
        findObjectDimensions(&(node->nodes[ii]), transformation, minDimension, maxDimension);
    }
}


