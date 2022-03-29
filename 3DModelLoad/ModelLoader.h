#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <QObject>

#include <QVector3D>
#include <QMatrix4x4>
#include <QVector4D>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

//结构MaterialInfo将包含有关材料外观的信息。我们将使用Phong着色模型(2)进行着色。
struct MaterialInfo{
    QString name;
    QVector3D ambient;
    QVector3D diffuse;
    QVector3D specular;
    float shiness;
};

//LightInfo结构将包含有关光源的信息：
struct LightInfo{
    QVector4D position;
    QVector3D intensity;
    QSharedPointer<MaterialInfo> material;
};


//Mesh类将为我们提供有关网格的信息。它实际上不包含网格的顶点数据，但是具有我们需要从顶点缓冲区中获取的信息。
//Mesh::indexCount是网格中的顶点数，Mesh::indexOffset是缓冲区中顶点数据开始的位置，Mesh::material是网格的材质信息。
struct Mesh{
    QString name;
    unsigned int indexCount;
    unsigned int indexOffset;
    QSharedPointer<MaterialInfo> material;
};

//单个模型可能具有许多不同的网格。 Node类将包含网格以及将其放置在场景中的转换矩阵。每个节点还可以具有子节点。
//我们可以将所有网格存储在单个数组中，但是将它们存储在树形结构中可以使我们更轻松地为对象设置动画。可以将其视为人体，
//就好像身体是根节点，上臂将是根节点的子节点，下臂将是上臂节点的子节点，而手将是下臂节点的子节点。
struct Node
{
    QString name;
    QMatrix4x4 transformation;
    QVector<QSharedPointer<Mesh> > meshes;
    QVector<Node> nodes;
};

class ModelLoader : public QObject
{
    Q_OBJECT
public:
    explicit ModelLoader(QObject *parent = nullptr);
    bool load(const QString path);
    void getBufferData(QVector<float> **vertices, QVector<float> **normals,
                       QVector<unsigned int> **indices);
    QSharedPointer<Node> getNodeData(){
        return m_rootNode;
    }
private:
    QSharedPointer<MaterialInfo> processMaterial(aiMaterial *material);
    QSharedPointer<Mesh> processMesh(aiMesh *mesh);
    void processNode(const aiScene *scene, aiNode *node, Node *parentNode, Node &newNode);

    void transformToUnitCoordinates();
    void findObjectDimensions(Node *node, QMatrix4x4 transformation, QVector3D &minDimension, QVector3D &maxDimension);

    QVector<float> m_vertices;
    QVector<float> m_normals;
    QVector<unsigned int> m_indices;

    QVector<QSharedPointer<MaterialInfo> > m_materials;
    QVector<QSharedPointer<Mesh> > m_meshes;
    QSharedPointer<Node> m_rootNode;
    bool m_transformToUnitCoordinates = true;
signals:

};

#endif // MODELLOADER_H
