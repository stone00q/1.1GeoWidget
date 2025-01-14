/**
 * @file    GeometryWidget.h
 * @brief   stp、step和iges文件的读入和显示
 * @details 主要接口函数即使用都在该文件中
*/
#ifndef GEOMETRYWIDGET_H
#define GEOMETRYWIDGET_H

#include <set>

#include <QVTKOpenGLNativeWidget.h>
#include <vtkInteractorStyleTrackballCamera.h>
class vtkOrientationMarkerWidget;
class vtkAxesActor;
class vtkLogoWidget;
class vtkCellPicker;
class vtkLookupTable;
class vtkOCCTReader;
class vtkThreshold;
class vtkDataSetMapper;
class vtkPolyData;
class GeometryWidget;


class HighlightInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static HighlightInteractorStyle* New();
    vtkTypeMacro(HighlightInteractorStyle, vtkInteractorStyleTrackballCamera);

    HighlightInteractorStyle();
    virtual void OnLeftButtonDown() override;
    //virtual void OnRightButtonDown() override;
    virtual void OnKeyPress() override;
    void SetInput(vtkSmartPointer<vtkPolyData> data,GeometryWidget* widget);
    std::set<int> GetSelectedSurfIds();
private:
    vtkSmartPointer<vtkPolyData> PolyData;
    vtkSmartPointer<vtkDataSetMapper> displayMapper;
    vtkSmartPointer<vtkActor> displayActor;
    vtkSmartPointer<vtkThreshold> threshold;
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> defaultStyle;
    GeometryWidget* geoWidget = nullptr;
    std::set<int> selectedSurfIds;
    unsigned int NumberOfClicks;
    int PreviousPosition[2];
    int ResetPixelDistance;
};

/**
 * @class GeometryWidget
 * @brief stp、step和iges文件的读入显示
 * @details 能够读取stp、step、iges数据并显示，可以开启或关闭根据所在面ID进行的颜色映射，能够鼠标左键选中面高亮显示
 */
class GeometryWidget : public QVTKOpenGLNativeWidget
{
    Q_OBJECT
public:
    explicit GeometryWidget(QWidget *parent = 0);
    ~GeometryWidget(){}
    /**
     * @brief 读取指定路径的数据并显示
     * @param QString fileName，文件所在路径
     * @details 读取的时候默认linearDeflection=0.1，angularDeflection=0.5
     */
    void SetInputData(QString fileName);

    /**
     * @brief SetDeflection精度相关设置
     * @param double linearDeflection，默认值是0.02，设置线性偏转
     * @param double angularDeflection，默认值是0.1，设置角度偏差
     * @param angularDeflection
     */
    void SetDeflection(double linearDeflection=0.02, double angularDeflection= 0.1);

    /**
     * @brief   开启高亮交互
     * @details 无默认参数
     * 交互方式为：鼠标左键选中高亮，"esc"键退出该高亮交互方式。当连续点击空白处时会清除所有高亮，按住ctrl+左键可以多选高亮。
     */
    void EnableHighlightMode();
    /**
     * @brief SetColorMapVisibility，是否进行面id颜色映射
     * @param bool flag默认是true，进行颜色映射，当设定为false时，关闭面ID颜色映射
     * @param QString propName，进行颜色映射的属性，默认是SurfID，即每个cell的面ID
     */
    void SetColorMapVisibility(bool flag = true, QString propName = "SurfID");

    /**
     * @brief GetSelectedSurfIds，获取当前选中的所有面id
     * @return set<int>，面id集合
     */
    std::set<int> GetSelectedSurfIds();
signals:
    /**
     * @brief SurfIdPicked
     * @param surfId在高亮交互下，鼠标左键选中高亮区域的面id
     * @details 发射信号：在高亮交互下，鼠标左键选中高亮区域的面id
     */
    void SurfIdPicked(int surfId);
private:
    vtkSmartPointer<vtkOCCTReader> geoReader;
    vtkSmartPointer<vtkPolyData> polyData;
    vtkSmartPointer<vtkPolyDataMapper> mapper;
    vtkSmartPointer<vtkLookupTable> lut;
    vtkSmartPointer<vtkActor> actor;
    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow;
    vtkSmartPointer<QVTKInteractor> qvtkInteractor;
    vtkSmartPointer<HighlightInteractorStyle> highlightStyle;
    vtkSmartPointer<vtkCellPicker> cellPicker;
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> defaultStyle;
    vtkSmartPointer<vtkLogoWidget> m_logoWidget;
    vtkSmartPointer<vtkOrientationMarkerWidget> m_orientationMarker;
    vtkSmartPointer<vtkAxesActor> m_axes;
    bool hasData = false;

};

#endif // GEOMETRYWIDGET_H
