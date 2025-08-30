#include "dashboardplugin.h"
#include <QLabel>
#include <QVBoxLayout>

// 插件的UI界面可以是一个独立的QWidget类，也可以在这里直接创建
class DashboardWidget : public QWidget {
public:
    DashboardWidget(QWidget* parent) : QWidget(parent) {
        auto layout = new QVBoxLayout(this);
        auto titleLabel = new QLabel("数据仪表盘", this);
        titleLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
        auto contentLabel = new QLabel("这里未来可以展示图表、统计数据等...", this);

        layout->addWidget(titleLabel);
        layout->addWidget(contentLabel);
        layout->addStretch();
    }
};


QString DashboardPlugin::name() const {
    return "仪表盘视图";
}

QIcon DashboardPlugin::icon() const {
    // 实际项目中应从资源文件加载图标
    return QIcon();
}

QWidget* DashboardPlugin::createWidget(QWidget* parent) {
    // 创建并返回我们的自定义UI控件
    return new DashboardWidget(parent);
}
