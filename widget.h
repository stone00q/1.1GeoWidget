#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QProcess>
#include "GeometryWidget.h"
#include <QDebug>
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    /**
     * @brief 高亮交互
     * @details 调用GeometryWidget类的EnableHighlightMode()函数实现，无参数
     * @memberof Widget
     */
    void highlightButton_clicked();

    /**
     * @brief 开启颜色映射
     * @details 调用GeometryWidget类的SetColorMapVisibility()函数实现，默认参数
     * @memberof Widget
     */
    void openColorMapButton_clicked();

    /**
     * @brief 关闭颜色映射
     * @details 调用GeometryWidget类的SetColorMapVisibility()函数实现，参数是FALSE
     * @memberof Widget
     */
    void closeColorMapButton_clicked();
    void setDeflectionButton_clicked();
    void handleSurfIdPicked(int surId)
    {
        qInfo()<< "emit信号接收：surfid "<<surId;
        //std::cout << "Selected SurfID: ";
    }
    void selectedId_clicked();
private:
    Ui::Widget *ui;
    GeometryWidget* m_geometryWidget;

};
#endif // WIDGET_H
