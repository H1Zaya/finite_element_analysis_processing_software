#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mydrawwidget.h"

#include <QBoxLayout>
#include <QTabBar>
#include <QSplitter>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QDateTime>
#include <QRegularExpression>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    //设置主窗口大小
    setFixedSize(800, 550);

    //设置窗口名称
    setWindowTitle("Zelement");

    //设置主widget
    QWidget* mainWidget = new QWidget(this);


    // 控件
    //设置主窗口左侧tabwidget
    QTabWidget* tabWidget = new QTabWidget(this);
    //4个tab为4个widget
    QWidget* tab1 = new QWidget();
    QWidget* tab3 = new QWidget();
    QWidget* tab4 = new QWidget();


    QString btnStyle = "QPushButton {"
                          "    background-color: #f2f4fa;"
                          "    color: #000000;"
                          "    border: 1px solid #8ed1e3;"
                          "    padding: 10px 20px;"
                          "    font-size: 16px;"
                          "    border-radius: 10px;"
                          "}"
                          "QPushButton:hover {"
                          "    background-color: #c3e0e7;"
                          "    color: #FFFFFF;"
                          "}"
                          "QPushButton:pressed {"
                          "    background-color: #8ed1e3;"
                          "}";

    QString btn2Style = "QPushButton {"
                          "    background-color: #f2f4fa;"
                          "    color: #000000;"
                          "    border: 1px solid #8ed1e3;"
                          "    padding: 5px 20px;"
                          "    font-size: 10px;"
                          "    font-weight: bold;"
                          "    border-radius: 5px;"
                          "}"
                          "QPushButton:hover {"
                          "    background-color: #c3e0e7;"
                          "    color: #FFFFFF;"
                          "}"
                          "QPushButton:pressed {"
                          "    background-color: #8ed1e3;"
                          "}";

    // 粗体字体
    QFont boldFont;
    boldFont.setBold(true);

    // 对4个widget将其丰富

    //  tab1 总体布局----------------------------------------------------//

    // button
    QPushButton* drawRect = new QPushButton("绘制矩");
    drawRect->setFixedWidth(80);
    QPushButton* drawCircle = new QPushButton("绘制圆");
    drawCircle->setFixedWidth(80);
    QPushButton* mouseRect = new QPushButton("Rectangle");
    mouseRect->setFixedWidth(80);
    QPushButton* mouseCircle = new QPushButton("Circle");
    mouseCircle->setFixedWidth(80);
    QPushButton* clearAll = new QPushButton("clear");
    clearAll->setFixedWidth(80);
    clearAll->setIcon(QIcon("C:/Qt/projects/alluse/icons/broom.png"));  // 设置按钮的图标
    clearAll->setIconSize(QSize(20, 20));

    // label  由于图标和文字无法正常出现在一个label里,这里选择放置一个水平布局来防止两个label
    QHBoxLayout* rectLabelLayout = new QHBoxLayout;
    QHBoxLayout* circleLabelLayout = new QHBoxLayout;
    QHBoxLayout* drawLabelLayout = new QHBoxLayout;

    QLabel* rectIconLabel = new QLabel("rectangle");
    QLabel* circleIconLabel = new QLabel("Circle");
    QLabel* drawIconLabel = new QLabel("draw");

    QLabel* rectLabel = new QLabel("rectangle");
    QLabel* circleLabel = new QLabel("Circle");
    QLabel* drawLabel = new QLabel("draw");

    rectLabel->setFont(boldFont);
    circleLabel->setFont(boldFont);
    drawLabel->setFont(boldFont);
    QPixmap rectPix("C:/Qt/projects/alluse/icons/frame.png");
    QPixmap circlePix("C:/Qt/projects/alluse/icons/rec.png");
    QPixmap drawPix("C:/Qt/projects/alluse/icons/paint-brush.png");
    QPixmap materialPix("C:/Qt/projects/alluse/icons/material.jpg");

    rectIconLabel->setPixmap(rectPix.scaled(QSize(20,20)));
    circleIconLabel->setPixmap(circlePix.scaled(QSize(20,20)));
    drawIconLabel->setPixmap(drawPix.scaled(QSize(20,20)));


    rectLabelLayout->addWidget(rectIconLabel);
    rectLabelLayout->addWidget(rectLabel);
    rectLabelLayout->addSpacing(200);
    circleLabelLayout->addWidget(circleIconLabel);
    circleLabelLayout->addWidget(circleLabel);
    circleLabelLayout->addSpacing(200);
    drawLabelLayout->addWidget(drawIconLabel);
    drawLabelLayout->addWidget(drawLabel);
    drawLabelLayout->addSpacing(200);


    // 材料参数模块
    QPushButton* addEniuBtn = new QPushButton("添加");
    addEniuBtn->setFixedWidth(80);
    QVBoxLayout* materialLayout = new QVBoxLayout;
    QLabel* materialLabel = new QLabel("构件材料");
    QLineEdit* ELine = new QLineEdit();
    ELine->setPlaceholderText("弹性模量");
    ELine->setFixedWidth(100);
    ELine->setText("200");

    QHBoxLayout* materiallabelLayout = new QHBoxLayout;  // 获取参数的两个编辑框
    QLabel* materialIconLabel = new QLabel();

    QLineEdit* niuLine = new QLineEdit();
    niuLine->setPlaceholderText("泊松比");
    niuLine->setFixedWidth(100);
    niuLine->setText("0.3");
    QHBoxLayout* eniuLayout = new QHBoxLayout;  // 获取参数的两个编辑框
    QHBoxLayout* eniuBtnLayout = new QHBoxLayout; // 放置添加参数按钮
    eniuLayout->addWidget(ELine);
    eniuLayout->addWidget(niuLine);
    eniuBtnLayout->addStretch();
    eniuBtnLayout->addWidget(addEniuBtn);
    materialIconLabel->setPixmap(materialPix.scaled(QSize(20,20)));
    materiallabelLayout->addWidget(materialIconLabel);
    materiallabelLayout->addWidget(materialLabel);
    materiallabelLayout->addSpacing(200);


    materialLayout->addLayout(materiallabelLayout);
    materialLayout->addLayout(eniuLayout);
    materialLayout->addLayout(eniuBtnLayout);




    // 布局
    QVBoxLayout* tab1Layout = new QVBoxLayout(tab1);
    QHBoxLayout* rzbLayout = new QHBoxLayout;// 矩形的起点坐标以及绘制按钮
    QHBoxLayout* lengthLayout = new QHBoxLayout;// 矩形的长和宽
    QHBoxLayout* czbLayout = new QHBoxLayout;// 圆的圆心和半径
    QHBoxLayout* rLayout = new QHBoxLayout;// 圆的绘制按钮
    QHBoxLayout* drawLayout = new QHBoxLayout;
    QHBoxLayout* clearLayout = new QHBoxLayout;

    QLineEdit* rzbLine = new QLineEdit;
    rzbLine->setPlaceholderText("左上坐标,逗号隔开");
    rzbLine->setFixedWidth(105);
    QLineEdit* lengthLine = new QLineEdit;
    lengthLine->setPlaceholderText("矩形的长");
    lengthLine->setFixedWidth(95);
    QLineEdit* widthLine = new QLineEdit;
    widthLine->setPlaceholderText("矩形的宽");
    widthLine->setFixedWidth(95);
    QLineEdit* czbLine = new QLineEdit;
    czbLine->setPlaceholderText("圆心坐标,逗号隔开");
    czbLine->setFixedWidth(105);
    QLineEdit* rLine = new QLineEdit;
    rLine->setPlaceholderText("圆形的半径");
    rLine->setFixedWidth(95);

    // 调试用默认值, 随时注释
    rzbLine->setText("50,50");
    lengthLine->setText("400");
    widthLine->setText("200");
    czbLine->setText("150,150");
    rLine->setText("50");

    addEniuBtn->setStyleSheet(btn2Style);
    drawCircle->setStyleSheet(btn2Style);
    drawRect->setStyleSheet(btn2Style);
    clearAll->setStyleSheet("QPushButton {"
                            "    background-color: #f2f4fa;"
                            "    color: #000000;"
                            "    border: 1px solid #8ed1e3;"
                            "    padding: 8px 10px;"
                            "    font-size: 10px;"
                            "    font-weight: bold;"
                            "    border-radius: 5px;"
                            "}"
                            "QPushButton:hover {"
                            "    background-color: #c3e0e7;"
                            "    color: #FFFFFF;"
                            "}"
                            "QPushButton:pressed {"
                            "    background-color: #8ed1e3;"
                            "}");




    // 向tab1中加入文本输入框
    lengthLayout->addWidget(lengthLine);//长度和宽度放在一行
    lengthLayout->addStretch();
    lengthLayout->addWidget(widthLine);
    rzbLayout->addWidget(rzbLine);// 坐标和按钮放在同一行
    rzbLayout->addStretch();
    rzbLayout->addWidget(drawRect);
    czbLayout->addWidget(czbLine);//圆形数据
    czbLayout->addStretch();
    czbLayout->addWidget(rLine);
    rLayout->addStretch();// 绘制圆形按钮
    rLayout->addWidget(drawCircle);
    drawLayout->addWidget(mouseRect);
    drawLayout->addStretch();
    drawLayout->addWidget(mouseCircle);
    clearLayout->addStretch();
    clearLayout->addWidget(clearAll);


    tab1Layout->addItem(new QSpacerItem(0, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));//距上层距离
    tab1Layout->addLayout(rectLabelLayout);
    tab1Layout->addLayout(lengthLayout);
    tab1Layout->addLayout(rzbLayout);
    tab1Layout->addLayout(circleLabelLayout);
    tab1Layout->addLayout(czbLayout);
    tab1Layout->addLayout(rLayout);
    tab1Layout->addItem(new QSpacerItem(0, 20, QSizePolicy::Minimum, QSizePolicy::Fixed));//手绘区距离数据区
//    tab1Layout->addLayout(drawLabelLayout);
//    tab1Layout->addLayout(drawLayout);
    tab1Layout->addLayout(materialLayout);
    tab1Layout->addItem(new QSpacerItem(0, 60, QSizePolicy::Minimum, QSizePolicy::Fixed));//按钮距下层距离
    tab1Layout->addLayout(clearLayout);
    tab1Layout->addItem(new QSpacerItem(0, 30, QSizePolicy::Minimum, QSizePolicy::Fixed));//按钮距下层距离

    tab1->setLayout(tab1Layout);

//**************************************************************************//


    // tab3 layout----------------------------------------------------//
    QVBoxLayout* tab3Layout = new QVBoxLayout(tab3);




    QPushButton* calcBtn = new QPushButton("calc");
    calcBtn->setIcon(QIcon("C:/Qt/projects/alluse/icons/calc.png"));  // 设置按钮的图标
    calcBtn->setIconSize(QSize(40, 40));
    QFont font("Arial", 12);  // 创建字体对象
    font.setBold(true);  // 将字体设置为粗体
    calcBtn->setFont(font);


    QPushButton* paintButton = new QPushButton("绘制");
    paintButton->setIcon(QIcon("C:/Qt/projects/alluse/icons/mesh2.png"));  // 设置按钮的图标
    paintButton->setIconSize(QSize(40, 40));
    paintButton->setFont(font);

    paintButton->setFixedWidth(150);

    calcBtn->setFixedHeight(100);
    calcBtn->setFixedWidth(150);
    paintButton->setFixedHeight(100);

    paintButton->setStyleSheet(btnStyle);


    calcBtn->setStyleSheet(btnStyle);

    QHBoxLayout* meshPaintLayout = new QHBoxLayout; // 放置划分网格按钮
    QHBoxLayout* calcLayout = new QHBoxLayout; // 放置划分网格按钮

    meshPaintLayout->addWidget(paintButton);
    calcLayout->addWidget(calcBtn);


    tab3Layout->addItem(new QSpacerItem(0, 50, QSizePolicy::Minimum, QSizePolicy::Fixed));//按钮距下层距离

    tab3Layout->addLayout(meshPaintLayout);
    tab3Layout->addItem(new QSpacerItem(0, 50, QSizePolicy::Minimum, QSizePolicy::Fixed));//按钮距下层距离

    tab3Layout->addLayout(calcLayout);
    tab3Layout->addItem(new QSpacerItem(0, 50, QSizePolicy::Minimum, QSizePolicy::Fixed));//按钮距下层距离

    tab3->setLayout(tab3Layout);
//**************************************************************************//




//**************************************************************************//


    // tab4 layout----------------------------------------------------//
    QPixmap strainPix("C:/Qt/projects/alluse/icons/strain.jpg");
    QPixmap stressPix("C:/Qt/projects/alluse/icons/stress.png");
    QPixmap displacementPix("C:/Qt/projects/alluse/icons/displacement.png");

    QLabel* strainPixLabel = new QLabel();
    QLabel* stressPixLabel = new QLabel();
    QLabel* displacementPixLabel = new QLabel();

    QLabel* strainLabel = new QLabel("strain");
    QLabel* stressLabel = new QLabel("stress");
    QLabel* displacementLabel = new QLabel("displacement");

    strainLabel->setFont(boldFont);
    stressLabel->setFont(boldFont);
    displacementLabel->setFont(boldFont);


    QHBoxLayout* strainlabelLay = new QHBoxLayout();
    QHBoxLayout* stresslabelLay = new QHBoxLayout();
    QHBoxLayout* dislabelLay = new QHBoxLayout();

    strainPixLabel->setPixmap(strainPix.scaled(QSize(20,20)));
    stressPixLabel->setPixmap(stressPix.scaled(QSize(20,20)));
    displacementPixLabel->setPixmap(displacementPix.scaled(QSize(20,20)));

    strainlabelLay->addWidget(strainPixLabel);
    strainlabelLay->addWidget(strainLabel);
    strainlabelLay->addSpacing(200);

    stresslabelLay->addWidget(stressPixLabel);
    stresslabelLay->addWidget(stressLabel);
    stresslabelLay->addSpacing(200);

    dislabelLay->addWidget(displacementPixLabel);
    dislabelLay->addWidget(displacementLabel);
    dislabelLay->addSpacing(200);


    QVBoxLayout* tab4Layout = new QVBoxLayout(tab4);
    QPushButton* disBtn = new QPushButton("变形");

    QHBoxLayout* strainLay = new QHBoxLayout();
    QHBoxLayout* stressLay = new QHBoxLayout();
    QHBoxLayout* disLay = new QHBoxLayout();


    QPushButton* strainBtn = new QPushButton("应变");
    QPushButton* stressBtn = new QPushButton("应力");

    stressBtn->setStyleSheet(btn2Style);
    strainBtn->setStyleSheet(btn2Style);
    disBtn->setStyleSheet(btn2Style);





    QLineEdit* strainLine = new QLineEdit();
    QLineEdit* stressLine = new QLineEdit();

    strainLine->setPlaceholderText("0,1,2");
    stressLine->setPlaceholderText("0,1,2");
    strainLine->setFixedWidth(100);
    stressLine->setFixedWidth(100);

    strainLay->addWidget(strainLine);
    strainLay->addStretch();
    strainLay->addWidget(strainBtn);

    stressLay->addWidget(stressLine);
    stressLay->addStretch();
    stressLay->addWidget(stressBtn);

    disLay->addStretch();
    disLay->addWidget(disBtn);

    tab4Layout->addItem(new QSpacerItem(0, 20, QSizePolicy::Minimum, QSizePolicy::Fixed));//按钮距下层距离

    tab4Layout->addLayout(dislabelLay);
    tab4Layout->addLayout(disLay);
    tab4Layout->addItem(new QSpacerItem(0, 50, QSizePolicy::Minimum, QSizePolicy::Fixed));//按钮距下层距离


    tab4Layout->addLayout(strainlabelLay);
    tab4Layout->addLayout(strainLay);
    tab4Layout->addItem(new QSpacerItem(0, 50, QSizePolicy::Minimum, QSizePolicy::Fixed));//按钮距下层距离


    tab4Layout->addLayout(stresslabelLay);
    tab4Layout->addLayout(stressLay);
    tab4Layout->addItem(new QSpacerItem(0, 80, QSizePolicy::Minimum, QSizePolicy::Fixed));//按钮距下层距离




    tab4->setLayout(tab4Layout);
//**************************************************************************//


    // 添加进tabwidget
    tabWidget->addTab(tab1, "绘图");
    tabWidget->addTab(tab3, "网格和计算");
    tabWidget->addTab(tab4, "计算结果展示");

    // 获取所有tab的宽度之和
    int tabWidth = 0;
    for (int i = 0; i < tabWidget->count(); i++) {
        tabWidth += tabWidget->tabBar()->tabRect(i).width();
    }
    // 设置QTabWidget的宽度为tab的宽度之和
    tabWidget->setFixedWidth(tabWidth);

    //新建一个普通widget
    MyDrawWidget* draw_widget = new MyDrawWidget(this);
    draw_widget->setFixedSize(550, 350);

    // 文本Widget
    textEdit = new QTextEdit;
    textEdit->setFixedSize(550,150);
    textEdit->setReadOnly(true);// read only
    textEdit->setText("soft");



    // 创建主布局
    QHBoxLayout* mainLayout = new QHBoxLayout(mainWidget);

    // 创建右侧布局
    QVBoxLayout* rightLayout = new QVBoxLayout();
    rightLayout->addWidget(draw_widget);
    rightLayout->addWidget(textEdit);


    // 将左侧widget添加到主布局中
    mainLayout->addWidget(tabWidget);

    // 将右侧布局添加到主布局中
    mainLayout->addLayout(rightLayout);

    // 设置主widget的布局
    mainWidget->setLayout(mainLayout);


    setCentralWidget(mainWidget);







    // 通过数据绘制矩形按钮
    connect(drawRect, &QPushButton::clicked, this, [=]() {
        getRectData(draw_widget,rzbLine,lengthLine,widthLine);
    });

    // 通过数据绘制圆形按钮
    connect(drawCircle, &QPushButton::clicked, this, [=]() {
        getCircleData(draw_widget,czbLine,rLine);
    });

    // 清除图像按钮
    connect(clearAll, &QPushButton::clicked, this, [=]() {
        clearPicButton(draw_widget);
    });

    // 网格绘制按钮
    connect(paintButton, &QPushButton::clicked, this, [=]() {
        doMesh(draw_widget);
    });

    // 获取材料数据按钮
    connect(addEniuBtn, &QPushButton::clicked, this, [=]() {
        getMaterialData(draw_widget,ELine,niuLine);
    });


    // 错误报告连接
    // 文字提示，链接mywidget和mainwindow信号
    connect(draw_widget, &MyDrawWidget::updateTextEdit, this, &MainWindow::onUpdateTextEdit);



    // 计算按钮
    connect(calcBtn, &QPushButton::clicked, this, [=]() {
        calculation(draw_widget);
    });

    // 位移呈现
    connect(disBtn, &QPushButton::clicked, this, [=]() {
        showdisplacement(draw_widget);
    });

    // 应变云图
    connect(strainBtn, &QPushButton::clicked, this, [=]() {
        showstrain(draw_widget,strainLine);
    });

    // 应力云图
    connect(stressBtn, &QPushButton::clicked, this, [=]() {
        showstress(draw_widget,stressLine);
    });



}

MainWindow::~MainWindow()
{
    delete ui;
}

// 初始化我的一个对象
QRegularExpression MainWindow::rx("\\d+,\\d+");
QRegularExpression MainWindow::rx2("\\d+");

// 获取矩形数据的函数  注意, 矩形的长度就是宽度, 宽度就是高度   绘制矩形按钮的函数
// 并且触发重绘
void MainWindow::getRectData(MyDrawWidget* widget,QLineEdit* lineEditXY,QLineEdit* lineEditW,QLineEdit* lineEditH) {

    // 判断编辑框内的数据是否符合要求
    if(lineEditXY->text().isEmpty() || lineEditW->text().isEmpty() || lineEditH->text().isEmpty())
    {
        updateTextEdit(1,"rectangle is incomplete.");
        return;
    }

    if(!(rx.match(lineEditXY->text()).hasMatch()))
    {
        updateTextEdit(1,"Coordinate format error.");
        return;
    }

    if((!(rx2.match(lineEditW->text()).hasMatch())) || (!(rx2.match(lineEditH->text()).hasMatch())))
    {
        updateTextEdit(1,"length or height of rectangle in not legal.");
        return;
    }

    QString xyStr = lineEditXY->text();
    QString wStr = lineEditW->text();
    QString hStr = lineEditH->text();

    double x = xyStr.split(",")[0].toDouble();
    double y = xyStr.split(",")[1].toDouble();
    double w = wStr.toDouble();
    double h = hStr.toDouble();

    QRectF rect(x, y, w, h);
    widget->drawRect(rect);
}


// 传递圆的参数，绘制圆按钮函数
// 并且触发重绘
void MainWindow::getCircleData(MyDrawWidget* widget,QLineEdit* lineEditXY,QLineEdit* lineEditR) {

    // 判断编辑框内的数据是否符合要求
    if(lineEditXY->text().isEmpty() || lineEditR->text().isEmpty())
    {
        updateTextEdit(1,"circle in not legal.");

        return;
    }

    if(!(rx.match(lineEditXY->text()).hasMatch()))
    {
        updateTextEdit(1,"circle in not legal 2.");

        return;
    }

    if(!(rx2.match(lineEditR->text()).hasMatch()))
    {
        updateTextEdit(1,"radius of circle in not legal.");
        return;
    }

    QString xyStr = lineEditXY->text();
    QString rStr = lineEditR->text();

    double x = xyStr.split(",")[0].toDouble();
    double y = xyStr.split(",")[1].toDouble();
    double r = rStr.toDouble();

    QPointF cir(x,y);

    widget->drawCircle(cir,r);
}

void MainWindow::getMaterialData(MyDrawWidget* widget, QLineEdit* lineEditE, QLineEdit* lineEditNiu) {

    // 初始化一个mymaterial对象
    MyMaterial material;
    if((!(rx2.match(lineEditE->text()).hasMatch())) || (!(rx2.match(lineEditNiu->text()).hasMatch())))
    {
        updateTextEdit(1,"incorrect format of E or niu.");
        return;
    }

    material.setE(lineEditE->text().toDouble());
    material.setNu(lineEditNiu->text().toDouble());
    if(material.getNu()<=0 || material.getNu()>=1)
    {
        updateTextEdit(1,"please input niu between 0~1.");
        return;
    }

    updateTextEdit(1,"材料参数已存储.");




    widget->storeMaterial(material);

}


// 清除按钮函数
void MainWindow::clearPicButton(MyDrawWidget* widget)
{
    widget->clearPixmap();
    widget->clearRectAndCircleData();

}

// 网格绘制按钮函数
void MainWindow::doMesh(MyDrawWidget* widget)
{
   widget->meshPaint();
}



// 文字提示 需配合传入的字符串, 信号链接mywidget
void MainWindow::onUpdateTextEdit(bool shouldUpdate, QString str)
{
    if(!(shouldUpdate))
    {
        QDateTime currentTime = QDateTime::currentDateTime(); // 添加当前时间
        QString line = currentTime.toString("hh:mm:ss")+": "+str; // 组合成句子
        textEdit->append(line); // 显示在右小角
    }
}


// 主页面更改文字提示函数
void MainWindow::updateTextEdit(bool upd, QString str)
{
    if(upd)
    {
        QDateTime currentTime = QDateTime::currentDateTime(); // 添加当前时间
        QString line = currentTime.toString("hh:mm:ss")+": "+str; // 组合成句子
        textEdit->append(line); // 显示在右小角
    }
}


// 计算
void MainWindow::calculation(MyDrawWidget* widget)
{
    widget->calcMatrix();
}


// 显示位移图
void MainWindow::showdisplacement(MyDrawWidget* widget){
    widget->drawDisplacement();
}


// 显示应变云图
void MainWindow::showstrain(MyDrawWidget* widget, QLineEdit* lineEdit){
    QString str = lineEdit->text();
    int k=-1;
    if(str=="0")
        k=0;
    if(str=="1")
        k=1;
    if(str=="2")
        k=2;
    if(k!=-1)
        widget->drawstrain(k);
    else
        updateTextEdit(1,"incorrect format.");}


// 显示应力云图
void MainWindow::showstress(MyDrawWidget* widget, QLineEdit* lineEdit){
    QString str = lineEdit->text();
    int k=-1;
    if(str=="0")
        k=0;
    if(str=="1")
        k=1;
    if(str=="2")
        k=2;
    if(k!=-1)
        widget->drawStress(k);
    else
        updateTextEdit(1,"incorrect format.");
}
