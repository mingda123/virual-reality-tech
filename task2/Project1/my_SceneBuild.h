#pragma once
#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osg/MatrixTransform>
#include <osg/ComputeBoundsVisitor>
#include <osg/Texture2D>
#include <osgUtil/SmoothingVisitor>
#include <osg/Material>
#include <osg/LightModel>
#include <vector>

using namespace std;

class SceneBuilder 
{
public:
    osg::ref_ptr<osg::Group> buildScene();

private:
    // 计算模型的高度
    float getModelHeight(osg::Node* model);
    // 加载OBJ模型并设置正确的位置
    osg::ref_ptr<osg::MatrixTransform> loadOBJModelWithPosition(const std::string& filename, float zOffset = 0.0f);
    void loadDEM();
    void loadTexture();
    osg::ref_ptr<osg::Transform> createTerrainGeode();


    int m_width, m_height;	
    vector<vector<int>> m_attitude;
    int m_simpler_rate = 2;           // 降采样倍率
    int m_center_height = 0;              // 地形中心点高度

    osg::ref_ptr<osg::Image> m_textureImage;
    osg::ref_ptr<osg::Texture2D> m_texture;
};