!contains(INCLUDEDFIES, assimp.pri) {
INCLUDEDFIES += assimp.pri

INCLUDEPATH += $$PWD \
$$PWD/include

LIBS += -L$$PWD/lib/Release \
    -lassimp-vc142-mt
}

