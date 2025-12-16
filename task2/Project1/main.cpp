#include <osgViewer/Viewer>
#include <osgUtil/Optimizer>
#include <osgGA/TrackballManipulator>
#include <iostream>

#include "my_SceneBuild.h"
#include "my_EventHandler.h"

int main()
{
    // 创建查看器
    osgViewer::Viewer viewer;

    // 构建场景
    SceneBuilder builder;
    osg::ref_ptr<osg::Group> root = builder.buildScene();

    // 优化场景图
    osgUtil::Optimizer optimizer;
    optimizer.optimize(root.get());

    // 设置场景数据
    viewer.setSceneData(root.get());

    // 设置相机操作器（支持旋转、缩放、平移）
    osg::ref_ptr<osgGA::TrackballManipulator> manipulator = new osgGA::TrackballManipulator;
    viewer.setCameraManipulator(manipulator);

    viewer.setUpViewInWindow(100, 100, 800, 600);

    // 设置初始视角
    viewer.getCameraManipulator()->setHomePosition(
        osg::Vec3d(0.0, 0, 4000),  // 眼睛位置
        osg::Vec3d(0.0, 0.0, 0),    // 观察点
        osg::Vec3d(0.0, 0.0, 1.0)     // 上方向
    );

    // 添加拾取事件处理器
    viewer.addEventHandler(new PickHandler(root));

    std::cout << "\n=== 操作说明 ===" << std::endl;
    std::cout << "鼠标左键拖拽: 旋转视图" << std::endl;
    std::cout << "鼠标中键拖拽: 平移视图" << std::endl;
    std::cout << "鼠标右键拖拽: 缩放视图" << std::endl;
    std::cout << "鼠标左键点击: 拾取几何实体" << std::endl;

    // 开始渲染循环
    return viewer.run();
}