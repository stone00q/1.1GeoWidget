#include "widget.h"
#include "ui_widget.h"
#include <qgridlayout.h>
#include <qpushbutton.h>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    //创建自定义widget实例
    m_geometryWidget = new GeometryWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_geometryWidget);
    //设置读取文件路径
    m_geometryWidget->SetInputData(R"(D:\Project\VTK_QT\data\c3x_blade.stp)");
    //按钮绑定高亮交互功能，在槽函数里调用class GeometryWidget的成员函数void EnableHighlightMode()
    QPushButton* highlightButton = new QPushButton("高亮交互",this);
    layout->addWidget(highlightButton);
    connect(highlightButton,&QPushButton::clicked,this,&Widget::highlightButton_clicked);
    //按钮绑定颜色映射功能
    QPushButton* openColorMapButton = new QPushButton("surfid colormap",this);
    layout->addWidget(openColorMapButton);
    connect(openColorMapButton,&QPushButton::clicked,this,&Widget::openColorMapButton_clicked);
    //取消颜色映射
    QPushButton* closeColorMapButton = new QPushButton("关闭颜色映射",this);
    layout->addWidget(closeColorMapButton);
    connect(closeColorMapButton,&QPushButton::clicked,this,&Widget::closeColorMapButton_clicked);

    connect(m_geometryWidget,&GeometryWidget::SurfIdPicked,this,&Widget::handleSurfIdPicked);
    QPushButton* setDeflectionButton=new QPushButton("linearDeflection=0.01,angularDeflection= 0.05",this);
    layout->addWidget(setDeflectionButton);
    connect(setDeflectionButton,&QPushButton::clicked,this,&Widget::setDeflectionButton_clicked);

    QPushButton* selectedId=new QPushButton("查看当前id",this);
    layout->addWidget(selectedId);
    connect(selectedId,&QPushButton::clicked,this,&Widget::selectedId_clicked);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::highlightButton_clicked()
{
    m_geometryWidget->EnableHighlightMode();
}
void Widget::openColorMapButton_clicked()
{
    m_geometryWidget->SetColorMapVisibility();
}
void Widget::closeColorMapButton_clicked()
{
    m_geometryWidget->SetColorMapVisibility(FALSE);
}
void Widget::setDeflectionButton_clicked()
{
    m_geometryWidget->SetDeflection();
}
void Widget::selectedId_clicked()
{
    auto ids=m_geometryWidget->GetSelectedSurfIds();
    qInfo()<<"\n now these ids are selected:";
    for(int i:ids)
    {
        qInfo()<<" "<<i;
    }
}
