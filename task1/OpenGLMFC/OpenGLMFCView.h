
// OpenGLMFCView.h: COpenGLMFCView 类的接口
//

#pragma once
#include<iostream>
#include<string>
#include<sstream>
#include<fstream>
#include<vector>
using namespace std;

class COpenGLMFCView : public CView
{
protected: // 仅从序列化创建
	COpenGLMFCView() noexcept;
	DECLARE_DYNCREATE(COpenGLMFCView)

// 特性
public:
	COpenGLMFCDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~COpenGLMFCView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

public:
// 自编程序
	HGLRC m_hRC;
	CClientDC* m_pDC;
	int m_width, m_height;
	int m_file_in = 0;

	vector<vector<int>> m_attitude;
	UINT m_glTexture = 0;
	bool m_projection_mode = 0;    // 是否采用透视投影(默认为正交投影)
	float m_rotateX = 0.0;
	float m_rotateY = 0.0;
	bool m_bRotating = false;      // 是否正在旋转
	POINT m_last_mouse_pos;        // 上次鼠标位置
	bool m_left_mouse = false;     // 鼠标左键是否按下
	float m_zoom = 1.0;            // 缩放比例
	// 高度夸张
	int m_up = 0;
	int m_down = 0;

	//初始化 OpenGL
	bool InitializeOpenGL(HDC hDC);
	//设置 DC 像素格式
	bool SetDCPixelFormat(HDC hDC);
	// 绘制DEM
	void DrawTerrain();

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDataload();
	afx_msg void OnTextureload();
	afx_msg void OnOrtho();
	afx_msg void OnPerspective();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnHeightlarger();
	afx_msg void OnHeightsmaller();
};

#ifndef _DEBUG  // OpenGLMFCView.cpp 中的调试版本
inline COpenGLMFCDoc* COpenGLMFCView::GetDocument() const
   { return reinterpret_cast<COpenGLMFCDoc*>(m_pDocument); }
#endif

