#include "GeometryWidget.h"

//#include <QWidget>
//#include <QMenu>
//#include <QCursor>
//#include <QDebug>
#include <vtkMultiBlockDataSet.h>
#include <vtkCellData.h>
#include <vtkDataSet.h>
#include <vtkProperty.h>
#include <vtkRendererCollection.h>
#include <vtkObjectFactory.h>
#include <vtkRenderWindow.h>
#include <vtkGeometryFilter.h>
#include <vtkAppendPolyData.h>
#include <vtkIntArray.h>
#include <vtkDataArray.h>
#include <vtkGenericOpenGLRenderWindow.h>

//#include <ctime>
#include <vtkOCCTReader.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPolyDataMapper.h>
#include <vtkLookupTable.h>
#include <vtkCellPicker.h>
#include <vtkSetGet.h>
#include <vtkDataSetMapper.h>
#include <vtkThreshold.h>

#include <vtkActor.h>
#include <vtkRenderer.h>

#include <vtkAutoInit.h>
#include <vtkLogoRepresentation.h>
#include <vtkLogoWidget.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPNGReader.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkAxesActor.h>

//#include <vtkAppendFilter.h>
//#include <vtkNew.h>
//#include <vtkCompositePolyDataMapper.h>
//#include <vtkIdList.h>
//#include <vtkIdTypeArray.h>
//#include <vtkExtractSelection.h>
//#include <vtkSelectionNode.h>
//#include <vtkSelection.h>

VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
vtkStandardNewMacro(HighlightInteractorStyle);
GeometryWidget::GeometryWidget(QWidget *parent)
    :QVTKOpenGLNativeWidget(parent)
{

    this->geoReader = vtkSmartPointer<vtkOCCTReader>::New();
    this->lut = vtkSmartPointer<vtkLookupTable>::New();
    this->renderer = vtkSmartPointer<vtkRenderer>::New();
    this->renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    this->renderWindow->AddRenderer(renderer);
    this->setRenderWindow(renderWindow);

    this->mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    this->actor = vtkSmartPointer<vtkActor>::New();
    this->actor->SetMapper(mapper);
    this->highlightStyle = vtkSmartPointer<HighlightInteractorStyle>::New();
    this->defaultStyle = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    this->cellPicker=vtkSmartPointer<vtkCellPicker>::New();
    //获取交互器
    this->qvtkInteractor = this->interactor();
    this->qvtkInteractor->SetInteractorStyle(this->defaultStyle);
    this->qvtkInteractor->SetPicker(this->cellPicker);

    // 创建 logo 小部件
    // 加载图像文件
    //const vtkSmartPointer<vtkPNGReader> imageReader = vtkSmartPointer<vtkPNGReader>::New();
    //imageReader->SetFileName(R"(./logo_titile.png)"); // 替换为你的图像路径
    //imageReader->Update();

    this->m_logoWidget = vtkSmartPointer<vtkLogoWidget>::New();
    m_logoWidget->SetProcessEvents(0);
    m_logoWidget->SetInteractor(renderWindow->GetInteractor());
    m_logoWidget->SetResizable(false);
    m_logoWidget->SetSelectable(false);

    m_logoWidget->On(); // 启用小部件

    // 设置 logo 位置（右上角）
    const vtkSmartPointer<vtkLogoRepresentation> logoRepresentation = vtkSmartPointer<vtkLogoRepresentation>::New();


    logoRepresentation->Highlight(1);
    logoRepresentation->SetProportionalResize(true);
    logoRepresentation->SetPosition(0.85, 0.75);
    logoRepresentation->SetPosition2(0.15, 0.2);
    //logoRepresentation->SetImage(imageReader->GetOutput());
    logoRepresentation->SetShowBorderToOff();

    logoRepresentation->GetImageProperty()->SetOpacity(0.7);
    m_logoWidget->SetRepresentation(logoRepresentation);

    m_axes = vtkSmartPointer<vtkAxesActor>::New();
    m_orientationMarker = vtkSmartPointer<vtkOrientationMarkerWidget>::New();

    m_orientationMarker->SetOrientationMarker(m_axes);
    m_orientationMarker->SetInteractor(renderWindow->GetInteractor());
    // m_orientationMarker->SetViewport(0.0, 0.0, 0.3, 0.3);
    m_orientationMarker->SetViewport(0.0, 0.0, 0.12, 0.4);
    m_orientationMarker->SetEnabled(1);
    m_orientationMarker->InteractiveOff();

    renderer->SetBackground2(1.0, 1.0, 1.0); // 设置页面底部颜色值
    renderer->SetBackground(0.529, 0.8078, 0.92157); // 设置页面顶部颜色值
    renderer->SetGradientBackground(true); // 开启渐变色背景设置
    //QColor lightBlue(82, 87, 110);
    //renderer->SetBackground(lightBlue.redF(), lightBlue.greenF(), lightBlue.blueF());
}
void GeometryWidget::SetInputData(QString fileName)
{
    //clock_t start = clock();
    this->geoReader->SetFileName(fileName.toStdString().c_str());
    if(fileName.endsWith(".step")||fileName.endsWith(".stp"))
    {
        this->geoReader->SetFileFormat(vtkOCCTReader::Format::STEP);// 执行读取操作
    }else if(fileName.endsWith(".iges")||fileName.endsWith(".igs"))
    {
        this->geoReader->SetFileFormat(vtkOCCTReader::Format::IGES);
    }else
    {
        cout<<"报错"<<endl;
    }
    this->hasData=true;
    this->geoReader->Update();
    vtkMultiBlockDataSet* multiBlock = vtkMultiBlockDataSet::SafeDownCast(this->geoReader->GetOutput());
    //clock_t end = clock();
    // 计算运行时间（以秒为单位）
    //double elapsed_time = static_cast<double>(end - start) / CLOCKS_PER_SEC;
    //std::cout<< "runtime:" << elapsed_time << " seconds" << std::endl;
    this->polyData = vtkPolyData::SafeDownCast(multiBlock->GetBlock(0));//取出第一块
    this->mapper->SetInputData(this->polyData);
    this->mapper->ScalarVisibilityOff();

    this->renderer->AddActor(this->actor);
    this->renderer->ResetCamera();
    this->renderWindow->Render();
}
void GeometryWidget::SetDeflection(double linearDeflection, double angularDeflection)
{
    if(!this->hasData) return;
    this->geoReader->SetLinearDeflection(linearDeflection);
    this->geoReader->SetAngularDeflection(angularDeflection);
    this->geoReader->Update();
    this->polyData = vtkPolyData::SafeDownCast(this->geoReader->GetOutput()->GetBlock(0));
    this->mapper->SetInputData(this->polyData);
    this->renderer->ResetCamera();
    this->renderWindow->Render();
}
void GeometryWidget::SetColorMapVisibility(bool flag, QString propName)
{
    if(flag)
    {
        this->mapper->ScalarVisibilityOn();  //标量可见
        this->mapper->SetScalarModeToUseCellData();
        int propId = this->polyData->GetCellData()->SetActiveScalars(propName.toStdString().c_str()); // 设置标量
        vtkDataArray* propData = this->polyData->GetCellData()->GetArray(propId);

        lut->SetNumberOfColors(256);
        this->lut->Build();
        // 设置查找表的颜色范围
        this->mapper->SetScalarRange(propData->GetRange());
        this->mapper->SetLookupTable(lut);
    }else
    {
        this->mapper->ScalarVisibilityOff();
    }

    this->renderer->ResetCamera();
    this->renderWindow->Render();
}
void GeometryWidget::EnableHighlightMode()
{
    this->highlightStyle->SetInput(this->polyData,this);
    this->qvtkInteractor->SetInteractorStyle(this->highlightStyle);
    this->qvtkInteractor->Initialize();
}
std::set<int> GeometryWidget::GetSelectedSurfIds()
{
    return this->highlightStyle->GetSelectedSurfIds();
}
HighlightInteractorStyle::HighlightInteractorStyle()
{
    this->defaultStyle = vtkSmartPointer< vtkInteractorStyleTrackballCamera>::New();
    this->threshold = vtkSmartPointer<vtkThreshold>::New();
    this->displayMapper = vtkSmartPointer<vtkDataSetMapper>::New();
    this->displayMapper->ScalarVisibilityOff();  // 禁用标量可见性
    this->displayActor = vtkSmartPointer<vtkActor>::New();
    this->displayActor->SetMapper(displayMapper);
    this->displayActor->GetProperty()->SetColor(1.0,0, 1);  // Highlighted color

    //this->lastClickWasBlank=false;
    //??this->lastClickTime=nullptr;
    this->NumberOfClicks=0;
    this->ResetPixelDistance=5;
    this->PreviousPosition[0]=0;
    this->PreviousPosition[1]=0;
}

void HighlightInteractorStyle::OnLeftButtonDown()
{
    //逻辑上，每次鼠标左键点击会拾取cellid并获得这个cell的surfid，然后发送surfid。
    //发送完检查是否按住了ctrl键，如果没按住就清空surfid的队列，即之前选中的id
    //将surid队列中的每个面去threshold抽出来用appendfilter连接成一个polydata去渲染
    //鼠标点击位置[x, y]   this->Picker = vtkCellPicker::SafeDownCast(this->GetInteractor()->GetPicker());
    int* clickPos = this->Interactor->GetEventPosition();

    // 进行point拾取
    auto picker = this->Interactor->GetPicker();
    picker->Pick(clickPos[0], clickPos[1], 0, this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());

    // 获取拾取到的cell ID
    auto cellPicker = vtkCellPicker::SafeDownCast(picker);
    auto cellId = cellPicker->GetCellId();
    if (cellId == -1) {
        this->NumberOfClicks++;
        int pickPostion[2];
        pickPostion[0]=clickPos[0];
        pickPostion[1]=clickPos[1];
        int xdist=pickPostion[0]-this->PreviousPosition[0];
        int ydist=pickPostion[1]-this->PreviousPosition[1];
        this->PreviousPosition[0]=pickPostion[0];
        this->PreviousPosition[1]=pickPostion[1];

        int moveDistance=(int)sqrt((double)(xdist*xdist+ydist*ydist));
        if(moveDistance>this->ResetPixelDistance)
        {
            this->NumberOfClicks=1;
        }
        if(this->NumberOfClicks==2)
        {
            this->NumberOfClicks=0;
            selectedSurfIds.clear();
            this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->RemoveActor(displayActor);
            this->Interactor->GetRenderWindow()->Render();
        }
        vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
        return;
    }
    auto surfIdArray = vtkIntArray::SafeDownCast(PolyData->GetCellData()->GetArray("SurfID"));
    if (!surfIdArray)
    {
        cout<<"SurfID array not found!"<<endl;
        return;
    }
    int pickedSurfId = surfIdArray->GetValue(cellId);
    //std::cout << "SurfID: " << pickedSurfId << std::endl;
    emit this->geoWidget->SurfIdPicked(pickedSurfId);

    //检查是否按下ctrl键
    bool ctrlPressed=this->Interactor->GetControlKey();
    if (selectedSurfIds.find(pickedSurfId)!=selectedSurfIds.end())
    {
        // 取消该面的高亮
        selectedSurfIds.erase(pickedSurfId);
    } else
    {
        if (!ctrlPressed) selectedSurfIds.clear(); //非多选模式
        selectedSurfIds.insert(pickedSurfId);
    }
    if(selectedSurfIds.empty())
    {
        this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->RemoveActor(displayActor);
        this->Interactor->GetRenderWindow()->Render();
        vtkInteractorStyleTrackballCamera::OnLeftButtonDown();  //事件转发
        return;
    }
    //下面开始提取相同surfid的cell
    vtkSmartPointer<vtkAppendPolyData> appendFilter = vtkSmartPointer<vtkAppendPolyData>::New();
    for (int surfId : selectedSurfIds) {
        threshold->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_CELLS, "SurfID");
        threshold->SetLowerThreshold(surfId);
        threshold->SetUpperThreshold(surfId);
        threshold->Update();
        vtkSmartPointer<vtkGeometryFilter> geometryFilter = vtkSmartPointer<vtkGeometryFilter>::New();
        geometryFilter->SetInputData(threshold->GetOutput());
        geometryFilter->Update();

        appendFilter->AddInputData(geometryFilter->GetOutput());
    }
    appendFilter->Update();
    displayMapper->SetInputConnection(appendFilter->GetOutputPort());
    this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(displayActor);
    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();  //事件转发
}
void HighlightInteractorStyle::OnKeyPress()
{
    // 获取按下的键
    std::string key = this->Interactor->GetKeySym();
    // 如果按下的是 "Escape" 键
    if (key == "Escape")
    {
        // 调用 InteractionManager 退出高亮模式
        if (this->Interactor)
        {
            this->selectedSurfIds.clear();
            this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->RemoveActor(displayActor);
            this->Interactor->GetRenderWindow()->Render();
            this->Interactor->SetInteractorStyle(defaultStyle);
        }
    }
    // 调用父类方法，处理其他按键
    vtkInteractorStyleTrackballCamera::OnKeyPress();
}
void HighlightInteractorStyle::SetInput(vtkSmartPointer<vtkPolyData> data,GeometryWidget* widget)
{
    this->PolyData = data;
    threshold->SetInputData(PolyData);
    geoWidget = widget;
}
std::set<int> HighlightInteractorStyle::GetSelectedSurfIds()
{
    return selectedSurfIds;
}

