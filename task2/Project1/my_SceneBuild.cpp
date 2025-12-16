#include <iostream>
#include "my_SceneBuild.h"

osg::ref_ptr<osg::Group> SceneBuilder::buildScene()
{
    osg::ref_ptr<osg::Group> root = new osg::Group;

    // 创造地形
    osg::ref_ptr<osg::Transform> terrain = createTerrainGeode();
    // 计算地形的高度
    if (terrain)
    {
        root->addChild(terrain);
        std::cout << "地形中心点高度: " << m_center_height << std::endl;
    }
    root->addChild(terrain);

    // 加载第一个楼层（放在地面）
    osg::ref_ptr<osg::MatrixTransform> floor1 = loadOBJModelWithPosition("Data/floor 3.obj", m_center_height + 0.0f);
    // 计算第一个楼层的高度
    float floor1Height = 0.0f;
    if (floor1)
    {
        // 获取第一个楼层的实际高度
        osg::ComputeBoundsVisitor boundVisitor;
        floor1->getChild(0)->accept(boundVisitor);
        osg::BoundingBox boundingBox = boundVisitor.getBoundingBox();
        floor1Height = boundingBox.zMax() - boundingBox.zMin();

        root->addChild(floor1);
        std::cout << "第一层楼高度: " << floor1Height << std::endl;
    }

    // 加载第二个楼层，放在第一个楼层的顶部
    osg::ref_ptr<osg::MatrixTransform> floor2 = loadOBJModelWithPosition("Data/floor 4.obj", m_center_height + floor1Height);
    if (floor2)
    {
        root->addChild(floor2);

        // 计算并显示第二个楼层的高度
        osg::ComputeBoundsVisitor boundVisitor;
        floor2->getChild(0)->accept(boundVisitor);
        osg::BoundingBox boundingBox = boundVisitor.getBoundingBox();
        float floor2Height = boundingBox.zMax() - boundingBox.zMin();
        std::cout << "第二层楼高度: " << floor2Height << std::endl;
        std::cout << "建筑总高度: " << (floor1Height + floor2Height) << std::endl;
    }

    return root;
}

// 计算模型的高度
float SceneBuilder::getModelHeight(osg::Node* model)
{
    if (!model) return 0.0f;

    osg::ComputeBoundsVisitor boundVisitor;
    model->accept(boundVisitor);
    osg::BoundingBox boundingBox = boundVisitor.getBoundingBox();

    return boundingBox.zMax() - boundingBox.zMin();
}

// 加载OBJ模型并设置正确的位置
osg::ref_ptr<osg::MatrixTransform> SceneBuilder::loadOBJModelWithPosition(const std::string& filename, float zOffset)
{
    osg::ref_ptr<osg::Node> model = osgDB::readNodeFile(filename);
    if (!model)
    {
        std::cout << "无法加载模型: " << filename << std::endl;
        return nullptr;
    }

    // 创建变换节点来定位模型
    osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform;
    transform->setName(filename);

    // 计算模型高度
    float height = getModelHeight(model.get());
    std::cout << "模型 " << filename << " 的高度: " << height << std::endl;

    // 设置位置矩阵 - 在Z轴方向偏移
    osg::Matrix matrix;
    matrix.makeTranslate(0.0f, 0.0f, zOffset);
    transform->setMatrix(matrix);
    transform->addChild(model);

    std::cout << "成功加载模型: " << filename << "，Z轴偏移: " << zOffset << std::endl;
    return transform;
}

// 加载DEM数据
void SceneBuilder::loadDEM()
{
    ifstream fin("./data/test.asc");
    string line_info, input_result;
    vector<string> info, data;
    int m = 0;
    if (!fin)
    {
        cout << "没有该文件！" << endl;
    }
    while (getline(fin, line_info))
    {
        m++;
        stringstream input(line_info);
        if (m <= 6)
        {
            while (input >> input_result)
                info.push_back(input_result);
        }
        else
        {
            while (input >> input_result)
                data.push_back(input_result);
        }

    }
    m_width = stoi(info[1]);
    m_height = stoi(info[3]);
    m_attitude.resize(m_height, vector<int>(m_width));
    for (int i = 0; i < m_height; i++)
    {
        for (int j = 0; j < m_width; j++)
        {
            m_attitude[i][j] = stoi(data[i * m_width + j]);
        }
    }
}

void SceneBuilder::loadTexture()
{
    m_textureImage = osgDB::readImageFile("./data/photo.jpg");

    if (m_textureImage.valid())
    {
        m_texture = new osg::Texture2D();
        m_texture->setImage(m_textureImage.get());

        // 设置纹理参数
        m_texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
        m_texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
        m_texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
        m_texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);

        cout << "纹理加载成功: " << m_textureImage->s() << " x " << m_textureImage->t() << endl;
    }
    else
    {
        cout << "纹理加载失败!" << endl;
    }
}

// 创建地形几何体
osg::ref_ptr<osg::Transform> SceneBuilder::createTerrainGeode()
{
    loadDEM();
    loadTexture();

    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array;

    // 地形降采样大小
    int simplifiedWidth = (m_width + 1) / m_simpler_rate;
    int simplifiedHeight = (m_height + 1) / m_simpler_rate;
    float heightScale = 0.1;
    float xyScale = 1;

    // 地形中心点高度
    int centerX = m_width / 2;
    int centerY = m_height / 2;
    centerX = std::max(0, std::min(m_width - 1, centerX));
    centerY = std::max(0, std::min(m_height - 1, centerY));
    m_center_height = m_attitude[centerY][centerX] * heightScale;

    // 生成顶点和纹理坐标
    for (int y = 0; y < simplifiedHeight; y++)
    {
        for (int x = 0; x < simplifiedWidth; x++)
        {
            // 计算原始DEM中的对应位置（每2x2区域取一个点）
            int origX = x * m_simpler_rate;
            int origY = y * m_simpler_rate;
            // 不越界
            if (origX >= m_width) origX = m_width - 1;
            if (origY >= m_height) origY = m_height - 1;

            float xPos = x * xyScale;
            float yPos = y * xyScale;
            float zPos = m_attitude[origY][origX] * heightScale;

            vertices->push_back(osg::Vec3(xPos, yPos, zPos));
            texcoords->push_back(osg::Vec2(
                static_cast<float>(x) / (simplifiedWidth - 1),
                static_cast<float>(y) / (simplifiedHeight - 1)
            ));
        }
    }

    geometry->setVertexArray(vertices);
    geometry->setTexCoordArray(0, texcoords);

    // 生成三角形索引
    osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(GL_TRIANGLES);
    for (int y = 0; y < simplifiedHeight - 1; y++)
    {
        for (int x = 0; x < simplifiedWidth - 1; x++)
        {
            int bl = y * simplifiedWidth + x;
            int br = bl + 1;
            int tl = (y + 1) * simplifiedWidth + x;
            int tr = tl + 1;

            indices->push_back(bl);
            indices->push_back(tl);
            indices->push_back(br);

            indices->push_back(br);
            indices->push_back(tl);
            indices->push_back(tr);
        }
    }

    geometry->addPrimitiveSet(indices);

    if (m_texture.valid())
    {
        osg::ref_ptr<osg::StateSet> stateset = geometry->getOrCreateStateSet();
        stateset->setTextureAttributeAndModes(0, m_texture.get());
    }

    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable(geometry);

    // 创建状态集
    osg::ref_ptr<osg::StateSet> stateset = geometry->getOrCreateStateSet();
    // 启用双面光照
    stateset->setMode(GL_LIGHTING, osg::StateAttribute::ON);
    stateset->setMode(GL_LIGHT0, osg::StateAttribute::ON);
    // 关闭背面剔除 - 显示两面
    stateset->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);

    // 设置光照模型为双面光照
    osg::ref_ptr<osg::LightModel> lightModel = new osg::LightModel;
    lightModel->setTwoSided(true);  
    stateset->setAttributeAndModes(lightModel.get(), osg::StateAttribute::ON);

    // 设置双面材质
    osg::ref_ptr<osg::Material> material = new osg::Material;
    material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0.4f, 0.4f, 0.4f, 1.0f));
    material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.9f, 0.9f, 0.9f, 1.0f));
    material->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.3f, 0.3f, 0.3f, 1.0f));
    material->setShininess(osg::Material::FRONT_AND_BACK, 20.0f);
    material->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);

    stateset->setAttributeAndModes(material.get(), osg::StateAttribute::ON);

    // 计算法线
    osgUtil::SmoothingVisitor smoothVisitor;
    smoothVisitor.smooth(*geometry);

    float centerX_world = (simplifiedWidth * xyScale) / 2.0f;
    float centerY_world = (simplifiedHeight * xyScale) / 2.0f;
    osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform;
    osg::Matrix matrix;
    matrix.makeTranslate(-centerX_world, -centerY_world, 0.0f); // 将中心移动到原点
    transform->setMatrix(matrix);
    transform->addChild(geode);

    cout << "地形创建完成" << endl;
    return transform;
}