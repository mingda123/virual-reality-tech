#pragma once
#include <osgGA/GUIEventHandler>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>
#include <osg/PolygonMode>
#include <osg/Material>
#include <iostream>


class PickHandler : public osgGA::GUIEventHandler
{
public:
    PickHandler(osg::Group* root) : _root(root) {}

    bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
    {
        if (ea.getEventType() != osgGA::GUIEventAdapter::RELEASE ||
            ea.getButton() != osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
        {
            return false;
        }

        osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
        if (!view) return false;

        // 创建线段相交检测器
        osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector =
            new osgUtil::LineSegmentIntersector(
                osgUtil::Intersector::WINDOW,
                ea.getX(), ea.getY());

        // 设置相交遍历选项
        osgUtil::IntersectionVisitor iv(intersector.get());
        iv.setTraversalMask(0x1); // 设置遍历掩码

        // 执行相交检测
        view->getCamera()->accept(iv);

        if (intersector->containsIntersections())
        {
            // 获取最近的交点
            osgUtil::LineSegmentIntersector::Intersection result =
                intersector->getFirstIntersection();

            // 获取被拾取的节点
            osg::Node* node = result.nodePath.back();

            // 高亮显示被拾取的物体
            highlightObject(node);

            // 打印拾取信息
            std::cout << "拾取到物体: " << node->getName() << std::endl;
            std::cout << "交点坐标: ("
                << result.getWorldIntersectPoint().x() << ", "
                << result.getWorldIntersectPoint().y() << ", "
                << result.getWorldIntersectPoint().z() << ")" << std::endl;

            return true;
        }

        return false;
    }

    // 高亮显示被拾取的物体
    void highlightObject(osg::Node* node)
    {
        // 清除之前的高亮
        clearHighlight();

        // 创建高亮状态集
        osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
        stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

        // 创建红色材质
        osg::ref_ptr<osg::Material> material = new osg::Material;
        material->setDiffuse(osg::Material::FRONT, osg::Vec4(1.0, 0.0, 0.0, 1.0));
        material->setAmbient(osg::Material::FRONT, osg::Vec4(0.1, 0.0, 0.0, 1.0));
        stateset->setAttributeAndModes(material.get(), osg::StateAttribute::ON);

        // 设置线框模式
        stateset->setAttributeAndModes(new osg::PolygonMode(
            osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));

        // 应用高亮效果
        node->setStateSet(stateset.get());
        _highlightedNode = node;
    }

    // 清除高亮效果
    void clearHighlight()
    {
        if (_highlightedNode.valid())
        {
            _highlightedNode->setStateSet(nullptr);
            _highlightedNode = nullptr;
        }
    }

private:
    osg::observer_ptr<osg::Group> _root;
    osg::observer_ptr<osg::Node> _highlightedNode;
};