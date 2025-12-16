
// OpenGLMFCView.cpp: COpenGLMFCView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "OpenGLMFC.h"
#endif

#include "OpenGLMFCDoc.h"
#include "OpenGLMFCView.h"

#define STBI_WINDOWS_UTF8
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// COpenGLMFCView

IMPLEMENT_DYNCREATE(COpenGLMFCView, CView)

BEGIN_MESSAGE_MAP(COpenGLMFCView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &COpenGLMFCView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_DATALOAD, &COpenGLMFCView::OnDataload)
	ON_COMMAND(ID_TextureLoad, &COpenGLMFCView::OnTextureload)
	ON_COMMAND(ID_Ortho, &COpenGLMFCView::OnOrtho)
	ON_COMMAND(ID_Perspective, &COpenGLMFCView::OnPerspective)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_HeightLarger, &COpenGLMFCView::OnHeightlarger)
	ON_COMMAND(ID_HeightSmaller, &COpenGLMFCView::OnHeightsmaller)
END_MESSAGE_MAP()

// COpenGLMFCView 构造/析构

COpenGLMFCView::COpenGLMFCView() noexcept
{
	// TODO: 在此处添加构造代码

}

COpenGLMFCView::~COpenGLMFCView()
{
}

BOOL COpenGLMFCView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// COpenGLMFCView 绘图

void COpenGLMFCView::OnDraw(CDC* /*pDC*/)
{
	COpenGLMFCDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	glClearColor(0.0f, 0.00f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	DrawTerrain();
	glFlush();
	SwapBuffers(wglGetCurrentDC());

}


// COpenGLMFCView 打印


void COpenGLMFCView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL COpenGLMFCView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void COpenGLMFCView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void COpenGLMFCView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void COpenGLMFCView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void COpenGLMFCView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// COpenGLMFCView 诊断

#ifdef _DEBUG
void COpenGLMFCView::AssertValid() const
{
	CView::AssertValid();
}

void COpenGLMFCView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

COpenGLMFCDoc* COpenGLMFCView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COpenGLMFCDoc)));
	return (COpenGLMFCDoc*)m_pDocument;
}
#endif //_DEBUG


// COpenGLMFCView 消息处理程序

bool COpenGLMFCView::InitializeOpenGL(HDC hDC)
{
	//设置 DC 像素格式
	if (false == SetDCPixelFormat(hDC))
	{
		return false;
	}

	//创建 RC
	HGLRC hRC = wglCreateContext(hDC);
	if (hRC == NULL)
	{
		return false;
	}
	//为当前线程设置 RC 
	if (wglMakeCurrent(hDC, hRC) == FALSE)
	{
		return false;
	}

	glClearDepth(1.0f);

	glEnable(GL_TEXTURE_2D);								// Enable Texture Mapping
	glEnable(GL_DEPTH_TEST);								// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);									// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);		// Really Nice Perspective Calculations
	return true;
}

bool COpenGLMFCView::SetDCPixelFormat(HDC hDC)
{
	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),	//pfd结构的大小
		1,								//版本号
		PFD_DRAW_TO_WINDOW |			//支持在窗口中绘图
		PFD_SUPPORT_OPENGL |			//支持OpenGL
		PFD_DOUBLEBUFFER,				//支持双缓冲
		PFD_TYPE_RGBA,					//RGBA颜色模式
		32,								//32位颜色深度
		0, 0, 0, 0, 0, 0,				//忽略颜色位
		0,								//没有非透明度缓存
		0,								//忽略移位位
		0,								//无累计缓存
		0, 0, 0, 0,						//忽略累计位
		32,								//32位深度缓存
		0,								//无模板缓存
		0,								//无辅助缓存
		PFD_MAIN_PLANE,					//主层
		0,								//保留
		0, 0, 0							//忽略层，可见性和损毁掩模
	};

	//得到 DC 最匹配的像素格式
	int pixelFormat = ChoosePixelFormat(hDC, &pfd);
	if (0 == pixelFormat)
	{
		//如果没有找到，就调用 DescribePixelFormat 函数来选择索引值为 1 的像素格式
		pixelFormat = 1;
		if (DescribePixelFormat(hDC, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd) == 0)
		{
			MessageBox(_T("ChoosePixelFormat 失败"));
			return false;
		}
	}
	//设置 DC 像素格式
	if (SetPixelFormat(hDC, pixelFormat, &pfd) == FALSE)
	{
		MessageBox(_T("SetPixelFormat 失败"));
		return false;
	}
	return true;
}
int COpenGLMFCView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	InitializeOpenGL(this->GetDC()->GetSafeHdc());

	return 0;
}

void COpenGLMFCView::DrawTerrain()
{

	glTranslatef(0, 0, -20);
	// 缩放
	glScaled(m_zoom, m_zoom, m_zoom);
	// 旋转
	glRotatef(m_rotateX, 1.0f, 0.0f, 0.0f);
	glRotatef(m_rotateY, 0.0f, 1.0f, 0.0f);

	//进行纹理映射
	glPointSize(1.0f);
	glShadeModel(GL_SMOOTH);

	//模型以及纹理的绘制
	for (int i = 0; i < m_height - 2; i++) {
		glBegin(GL_TRIANGLE_STRIP); // 三角形带
		for (int j = 0; j < m_width; j++) {
			if (m_attitude[i][j] != 0 && m_attitude[i + 1][j] != 0 && m_attitude[i + 2][j] != 0)
			{
				glTexCoord2f(float(i) / m_height, 1 - float(j) / m_width);
				glVertex3f(float(i - m_height / 2) / 100, float(j - m_width / 2) / 100, float(m_attitude[i][j]) / (1000 - 50 * (m_up - m_down)));
				glTexCoord2f(float(i + 1) / m_height, 1 - float(j + 1) / m_width);
				glVertex3f(float(i + 1 - m_height / 2) / 100, float(j - m_width / 2) / 100, float(m_attitude[i + 1][j]) / (1000 - 50 * (m_up - m_down)));
				glTexCoord2f(float(i + 2) / m_height, 1 - float(j + 2) / m_width);
				glVertex3f(float(i + 2 - m_height / 2) / 100, float(j - m_width / 2) / 100, float(m_attitude[i + 2][j]) / (1000 - 50 * (m_up - m_down)));
			}
		}
		glEnd();
	}
}
void COpenGLMFCView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	glViewport(0, 0, cx, cy);
	glMatrixMode(GL_PROJECTION);		//重置投影矩阵，告诉OpenGL接下来做投影变换
	glLoadIdentity();

	if (m_projection_mode == 0)
	{
		glOrtho(-8, 8, -8, 8, -100, 100);//正射投影
	}
	if (m_projection_mode == 1)
	{
		gluPerspective(45.0f, (GLfloat)cx / (GLfloat)cy, 1.0f, 100.0f);//透视投影
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Invalidate();
}

void COpenGLMFCView::OnDataload()
{
	// TODO: 在此添加命令处理程序代码
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
	m_file_in = 1;
	Invalidate();
}

void COpenGLMFCView::OnTextureload()
{
	// TODO: 在此添加命令处理程序代码
	stbi_set_flip_vertically_on_load(true);
	//载入纹理
	int width, height, nrChannels;
	unsigned char* data = stbi_load("./data/photo.jpg", &width, &height, &nrChannels, 0);
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// 为当前绑定的纹理对象设置环绕、过滤方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glEnable(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	Invalidate();
}

void COpenGLMFCView::OnOrtho()
{
	// TODO: 在此添加命令处理程序代码
	CRect rect;
	GetClientRect(&rect);
	int cx = rect.right;
	int cy = rect.bottom;
	glViewport(0, 0, cx, cy);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-8, 8, -8, 8, -100, 100);//正射投影
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 10, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glScaled(1.5, 1.5, 1.5);
	m_projection_mode = false;
	Invalidate();
}

void COpenGLMFCView::OnPerspective()
{
	// TODO: 在此添加命令处理程序代码
	CRect rect;
	GetClientRect(&rect);
	int cx = rect.right;
	int cy = rect.bottom;
	glViewport(0, 0, cx, cy);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)cx / (GLfloat)cy, 0.1f, 100.0f);//透视投影
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(8, 8, 5, 0.0, 0.0, 0.0, 0.0, 1, 0);
	m_projection_mode = true;
	Invalidate();
}

void COpenGLMFCView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect rect;
	GetClientRect(&rect);
	if (m_bRotating) {
		m_rotateY += (point.x - m_last_mouse_pos.x) * 0.1f;
		m_rotateX += (point.y - m_last_mouse_pos.y) * 0.1f;
		m_last_mouse_pos = point;
		if (m_last_mouse_pos.x <= rect.left + 10 || m_last_mouse_pos.x >= rect.right - 10 || m_last_mouse_pos.y <= rect.top + 10 || m_last_mouse_pos.y >= rect.bottom - 10)
		{
			m_left_mouse = false;
			m_bRotating - false;
		}
		Invalidate(FALSE);
	}

	CView::OnMouseMove(nFlags, point);
}

void COpenGLMFCView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_last_mouse_pos = point;
	m_bRotating = true;
	m_left_mouse = true;
	SetCapture();
	CView::OnLButtonDown(nFlags, point);
}

void COpenGLMFCView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_left_mouse = false;
	m_bRotating = false;
	ReleaseCapture();
	CView::OnLButtonUp(nFlags, point);
}

BOOL COpenGLMFCView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_zoom += zDelta * 0.0005f;
	if (m_zoom < 0.001f) m_zoom = 0.001f;
	if (m_zoom > 10.0f) m_zoom = 10.0f;
	Invalidate(FALSE);
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void COpenGLMFCView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nChar == VK_SPACE) {
		m_rotateX = 0.0;
		m_rotateY = 0.0;
		m_zoom = 1;
		m_up = 0;
		m_down = 0;
		Invalidate(FALSE);
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void COpenGLMFCView::OnHeightlarger()
{
	// TODO: 在此添加命令处理程序代码
	m_up++;
	Invalidate(FALSE);
}

void COpenGLMFCView::OnHeightsmaller()
{
	// TODO: 在此添加命令处理程序代码
	m_down++;
	Invalidate(FALSE);
}
