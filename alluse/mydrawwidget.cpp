#include "mydrawwidget.h"
#include "calc.h"

#include <QPainter>
#include <QToolTip>
#include <QPointF>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QRegularExpression>
#include <QEventLoop>
#include <QPushButton>

using namespace std;


MyDrawWidget::MyDrawWidget(QWidget *parent) : QWidget(parent)
{
    setMouseTracking(true);
    sig=0; // 判断是矩形的哪个顶点
    sig1=0;
    sig2=0; // 判断哪个边

    myNodes.reserve(9999);
    myElem.reserve(9999);
    MyForce zero(0,QPointF(0,0),0.0,0.0);

    addCons.resize(10000, 0);
    addForce.resize(10000, 0);

    pixmapnum = 16;

//    pixmapsign = {0};

    min = {0};
    max = {0};

    beforecalc=1;

}


// 绘图功能
void MyDrawWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true); // 抗锯齿
    painter.fillRect(rect(), Qt::white);  // 绘制背景颜色
    painter.setPen(QPen(Qt::black, 1)); // 设置画笔颜色和线宽

    int margin = 10; // 留出边距
    int width = 50;
    int height = 50;

    // 绘制 x 轴
    painter.drawLine(margin, margin, margin + width, margin);

    // 绘制 y 轴
    painter.drawLine(margin, margin + height, margin, margin);

    // 绘制原点
    QPoint origin(margin, margin);
    painter.setPen(QPen(Qt::black, 5)); // 设置画笔颜色和线宽
    painter.drawPoint(origin);

    // 绘制箭头



    for(int j = 0; j < 100; j++)
    {
        painter.drawPixmap(0,0,pixmapArray[j]);
    }

}


// 鼠标移动
void MyDrawWidget::mouseMoveEvent(QMouseEvent *event)
{
    // 获取当前位置坐标
    int x = event->position().rx();
    int y = event->position().ry();
    QString text = QString("X: %1, Y: %2").arg(x).arg(y);
    QToolTip::showText(event->globalPos(), text, this);


}



// 定义widget的绘画函数
void MyDrawWidget::drawRect(QRectF& rect) {

    // 判断是否为空  调试用 没作用
    if(rect.isNull() || rect.isEmpty())
    {
        qDebug() << "rect is empty";
        return;
    }

    QPixmap pixmap(size());
    pixmap.fill(Qt::transparent); // 用透明色填充整个pixmap

    QPainter painter(&pixmap);
    painter.setPen(Qt::red);
    // 存储矩形对象

    m_rect = rect;


    painter.drawRect(m_rect);
    pixmapArray[0] = pixmap; // 保存绘制完成的QPixmap


    // 重绘widget
    update();
}


void MyDrawWidget::drawCircle(QPointF& center_draw, double r_draw)
{

    if(m_rect.isNull())
    {
        emit updateTextEdit(0, "rectangle first please.");
        return;

    }

    QPixmap pixmap(size());
    pixmap.fill(Qt::transparent); // 用透明色填充整个pixmap

    QPainter painter(&pixmap);
    painter.setPen(Qt::red);


    if(m_center.isNull())
    {


        if(isCircleInRect(center_draw, r_draw))
        {
            painter.drawEllipse(center_draw,r_draw,r_draw);
            pixmapArray[1] = pixmap; // 保存绘制完成的QPixmap
            // 存储圆形对象
            m_center = center_draw;
            m_radius = r_draw;
        }
        QString strMsg = "circle is not in rectangle, please retry.";

        emit updateTextEdit(isCircleInRect(center_draw, r_draw), strMsg);

        // 重绘widget
        update();
    }else if((m_center == center_draw)&&(m_radius == r_draw)){
        QString strMsg = "same circle, please retry.";
        emit updateTextEdit(0, strMsg);

    }else{



        // 判断是否相交
        double distance = hypot(center_draw.x() - m_center.x(), center_draw.y() - m_center.y());
        bool intersect = distance <= (r_draw + m_radius);// 相交则intersect为1;

        if(isCircleInRect(center_draw, r_draw))
        {
            if(!intersect){
                painter.drawEllipse(center_draw,r_draw,r_draw);
                pixmapArray[2] = pixmap; // 保存绘制完成的QPixmap

                // 存储圆形对象2
                m_center2 = center_draw;
                m_radius2 = r_draw;
            }
            QString strMsg2 = "circles are intersecting, please retry.";
            emit updateTextEdit(!intersect, strMsg2);

        }
        QString strMsg = "circle is not in rectangle, please retry.";

        emit updateTextEdit(isCircleInRect(center_draw, r_draw), strMsg);


        qDebug() << m_center2.x();
        // 重绘widget
        update();
    }


}


// 清除矩形和圆的数据还有网格数据
void MyDrawWidget::clearRectAndCircleData()
{
    // 矩形圆形数据
    m_rect = QRectF();
    m_center = QPointF();
    m_radius = 0;
    m_center2 = QPointF();
    m_radius2 = 0;

    // 网格数据
    myNodes.clear();
    myElem.clear();
    nodes2.clear();
    delta.clear();

    min={0};
    max={0};

    // 力约束
    forceInfo.clear();
    vertexForce.clear();
    addForce.clear();
    addCons.clear();

    // 材料数据
    m_material.setE(0);
    m_material.setNu(0);

    // 力和约束数据


    beforecalc=1;

    update();
}

// 清除矩形和圆的图像
void MyDrawWidget::clearPixmap()
{

    for(int i = 0;i<100;i++)
    {
        pixmapArray[i].fill(QColor::fromRgbF(0, 0, 0, 0));
    }

    update();
}

// 判断图像是否符合工程实际,即圆要完全被矩形包围
bool MyDrawWidget::isCircleInRect(QPointF& point1, double r)
{
    if(m_rect.isNull() || point1.isNull())
    {
        return true;
    }
    if(m_rect.contains(point1))
    {

        double circleX = point1.rx();
        double circleY = point1.ry();

        double left = m_rect.left()-circleX;
        double right = m_rect.right()-circleX;
        double top = m_rect.top()-circleY;
        double bottom = m_rect.bottom()-circleY;

        // 判断矩形是否在圆内核心部分
        if(qAbs(left)>r && qAbs(right)>r && qAbs(top)>r && qAbs(bottom)>r)
            return true;

    }
    return false;
}


// 绘制网格
void MyDrawWidget::meshPaint()
{

    myNodes.clear();
    myElem.clear();


    vector<double> nodeCoords, parametricCoord;
    vector<size_t> nodeTags;
    int dim = -1;
    int tag = -1;
    bool includeBoundary = false;
    bool returnParametricCoords = true;

    vector<int> elementTypes;
    vector<vector<size_t>> elementTags;
    vector<vector<size_t>> nodeTags_2;

    gmsh::initialize();
    gmsh::option::setNumber("Mesh.Algorithm", 6);// 设置为 Delaunay 算法
    // 设置网格大小10
    gmsh::option::setNumber("Mesh.CharacteristicLengthMin", 10);
    gmsh::option::setNumber("Mesh.CharacteristicLengthMax", 10);

//    gmsh::option::setNumber("Mesh.Optimize", 1); // 优化网格
//    gmsh::option::setNumber("Mesh.Smoothing", 100); // 平滑网格
    gmsh::model::add("example");


    // 增加判断可以多绘制一种情况, 并且添加异常处理
    int temp = 0;
    int temp2 = 0;
    int temp3 = 0;
    double lc = 10;

    if(!(m_rect.isNull() || m_rect.isEmpty()))
    {
        temp = 1;
    }
    if(!m_center.isNull())
    {
        temp2 = 1;
    }
    if(!m_center2.isNull())
    {
        temp3 = 1;
    }


    if(temp>temp2) // 只有一个矩形, 可以进行绘制
    {
        double rectLeft = m_rect.left();
        double rectRight = m_rect.right();
        double rectTop = m_rect.top();
        double rectbottom = m_rect.bottom();



        // 将顶点坐标转换为点对象
        gmsh::model::occ::addPoint(rectLeft, rectTop, 0, lc, 1);
        gmsh::model::occ::addPoint(rectRight, rectTop, 0, lc, 2);
        gmsh::model::occ::addPoint(rectRight, rectbottom, 0, lc, 3);
        gmsh::model::occ::addPoint(rectLeft, rectbottom, 0, lc, 4);
        // 添加矩形边界
        gmsh::model::occ::addLine(1, 2, 1);
        gmsh::model::occ::addLine(2, 3, 2);
        gmsh::model::occ::addLine(3, 4, 3);
        gmsh::model::occ::addLine(4, 1, 4);
        gmsh::model::occ::addCurveLoop({1, 2, 3, 4}, 1);

        // 定义矩形截面
        gmsh::model::occ::addPlaneSurface({1}, 1);
    }else if((temp2==1) && (temp==1) && (temp3==0)) //同时有矩形和一个圆形, 可以进行绘制
    {
        double rectLeft = m_rect.left();
        double rectRight = m_rect.right();
        double rectTop = m_rect.top();
        double rectbottom = m_rect.bottom();


        double circleX = m_center.x();
        double circleY = m_center.y();


        // 将顶点坐标转换为点对象
        gmsh::model::occ::addPoint(rectLeft, rectTop, 0, lc, 1);
        gmsh::model::occ::addPoint(rectRight, rectTop, 0, lc, 2);
        gmsh::model::occ::addPoint(rectRight, rectbottom, 0, lc, 3);
        gmsh::model::occ::addPoint(rectLeft, rectbottom, 0, lc, 4);
        // 添加矩形边界
        gmsh::model::occ::addLine(1, 2, 1);
        gmsh::model::occ::addLine(2, 3, 2);
        gmsh::model::occ::addLine(3, 4, 3);
        gmsh::model::occ::addLine(4, 1, 4);
        gmsh::model::occ::addCurveLoop({1, 2, 3, 4}, 1);

        // 定义圆形边界
        gmsh::model::occ::addPoint(circleX, circleY, 0, lc, 5);
        gmsh::model::occ::addCircle(circleX, circleY, lc, m_radius);
        gmsh::model::occ::addCurveLoop({5}, 3);

        // 定义矩形减去中间圆形的面
        gmsh::model::occ::addPlaneSurface({1,3}, 1);
    }else if((temp2==1) && (temp==1) && (temp3==1)) // 两个圆一个矩形
    {

        double rectLeft = m_rect.left();
        double rectRight = m_rect.right();
        double rectTop = m_rect.top();
        double rectbottom = m_rect.bottom();


        double circleX = m_center.x();
        double circleY = m_center.y();

        double circleX2 = m_center2.x();
        double circleY2 = m_center2.y();


        // 将顶点坐标转换为点对象
        gmsh::model::geo::addPoint(rectLeft, rectTop, 0, lc, 1);
        gmsh::model::geo::addPoint(rectRight, rectTop, 0, lc, 2);
        gmsh::model::geo::addPoint(rectRight, rectbottom, 0, lc, 3);
        gmsh::model::geo::addPoint(rectLeft, rectbottom, 0, lc, 4);
        // 添加矩形边界
        gmsh::model::geo::addLine(1, 2, 1);
        gmsh::model::geo::addLine(2, 3, 2);
        gmsh::model::geo::addLine(3, 4, 3);
        gmsh::model::geo::addLine(4, 1, 4);
        gmsh::model::geo::addCurveLoop({1, 2, 3, 4}, 1);


        // 圆形相关
        gmsh::model::geo::addPoint(circleX, circleY, 0, lc, 5);
        gmsh::model::geo::addPoint(circleX-m_radius, circleY, 0, lc, 6);
        gmsh::model::geo::addPoint(circleX+m_radius, circleY, 0, lc, 8);
        gmsh::model::geo::addPoint(circleX, circleY-m_radius, 0, lc, 7);
        gmsh::model::geo::addPoint(circleX, circleY+m_radius, 0, lc, 9);

        gmsh::model::geo::addPoint(circleX2, circleY2, 0, lc, 10);
        gmsh::model::geo::addPoint(circleX2+m_radius2, circleY2, 0, lc, 11);
        gmsh::model::geo::addPoint(circleX2, circleY2+m_radius2, 0, lc, 12);
        gmsh::model::geo::addPoint(circleX2-m_radius2, circleY2, 0, lc, 13);
        gmsh::model::geo::addPoint(circleX2, circleY2-m_radius2, 0, lc, 14);

        gmsh::model::geo::addCircleArc(6,5,7,10);
        gmsh::model::geo::addCircleArc(7,5,8,11);
        gmsh::model::geo::addCircleArc(8,5,9,12);
        gmsh::model::geo::addCircleArc(9,5,6,13);
        gmsh::model::geo::addCurveLoop({10,11,12,13}, 2);

        gmsh::model::geo::addCircleArc(11,10,12,14);
        gmsh::model::geo::addCircleArc(12,10,13,15);
        gmsh::model::geo::addCircleArc(13,10,14,16);
        gmsh::model::geo::addCircleArc(14,10,11,17);
        gmsh::model::geo::addCurveLoop({14,15,16,17}, 3);

        // 定义矩形减去中间圆形的面
        gmsh::model::geo::addPlaneSurface({1,2,3}, 1);


    }else // 如果没有图形或者只有一个圆, 则不进行绘制
    {
        QString tStr = "ERROR: 如果没有图形或者只有一个圆, 则不进行绘制";
        emit updateTextEdit(0, tStr);
        gmsh::finalize();
        return;
    }

    // 生成几何实体
    gmsh::model::occ::synchronize();


    gmsh::model::geo::synchronize();

    // 生成网格
    gmsh::model::mesh::generate(2); // 生成整个模型的网格

    // 使用以下代码显示网格
//    gmsh::fltk::run();

    // 保存网格
    gmsh::write("output.msh");





    // 存储节点信息
    gmsh::model::mesh::getNodes(nodeTags, nodeCoords,parametricCoord, dim,tag,includeBoundary, returnParametricCoords);

    myPoint tempNode;
    // 遍历节点信息，并将其存储到 myNodes 容器中
    for (int i = 0; i < (int)nodeCoords.size(); i+=3) {

        tempNode.setX(nodeCoords[i]);
        tempNode.setY(nodeCoords[i + 1]);
        tempNode.setId(myNodes.size());
        myNodes.append(tempNode);
    }
    qDebug()<<myNodes.size()<<"\n";


    // 储存三角形信息
    gmsh::model::mesh::getElements(elementTypes, elementTags, nodeTags_2, dim, tag);
    myTriangle tempTriangle;
    int j=0;
    for(int i=0;i<(int)nodeTags_2[1].size();i+=3){
        tempTriangle.setId(j++);
        tempTriangle.setNode1((int)nodeTags_2[1][i+0]-1);
        tempTriangle.setNode2((int)nodeTags_2[1][i+1]-1);
        tempTriangle.setNode3((int)nodeTags_2[1][i+2]-1);
        myElem.append(tempTriangle);
    }
    qDebug()<<myElem.size()<<"\n";


    QPixmap pixmap(size());
    pixmap.fill(Qt::transparent); // 用透明色填充整个pixmap

    QPainter painter(&pixmap);
    // 绘制节点
    painter.setPen(Qt::blue);
    painter.setBrush(Qt::blue);
    for (const myPoint& node : myNodes) {
        // 根据节点的位置绘制一个小圆点
        painter.drawEllipse(node.getX() - 1, node.getY() - 1, 2, 2);
    }

    // 绘制三角形单元
    painter.setPen(Qt::darkGray);
    painter.setBrush(Qt::NoBrush);
    for (const myTriangle& triangle : myElem) {
        // 获取三角形单元的节点索引
        int index1 = triangle.getNode1();
        int index2 = triangle.getNode2();
        int index3 = triangle.getNode3();

        // 获取对应的节点坐标
        QPointF point1 = myNodes[index1].toPointF();
        QPointF point2 = myNodes[index2].toPointF();
        QPointF point3 = myNodes[index3].toPointF();

        // 绘制三角形
        QPolygonF polygon;
        polygon << point1 << point2 << point3;
        painter.drawPolygon(polygon);
    }
    pixmapArray[3] = pixmap;


    update();

    gmsh::finalize();


}


void MyDrawWidget::storeMaterial(MyMaterial& materialss)
{
    m_material = materialss;
    qDebug()<<m_material.getE();
}


// 点击事件重写
void MyDrawWidget::mousePressEvent(QMouseEvent* event) {

    if(myNodes.isEmpty())
    {
        return;
    }


    if(beforecalc)
    {
        QPointF currentPos;
        sig=0;
        sig1=0;
        sig2=0;

        // 将矩形四个顶点放入数组
        QPointF vertices[4];
        vertices[0] = m_rect.topLeft();
        vertices[1] = m_rect.topRight();
        vertices[2] = m_rect.bottomLeft();
        vertices[3] = m_rect.bottomRight();

        if (event->button() == Qt::LeftButton) // 是否是鼠标左键点击
        {
            // 获取鼠标点击位置
            QPointF pos = event->pos();

            // 判断是否点击到某个顶点
            if (isVertexClicked(pos)) {
                // 弹出窗口并处理参数
                vertexClickReaction();
                sig =0;
            }

            // 其他判断
            // todo
        }else if(event->button() == Qt::RightButton) // 右键点击添加集中力
        {
            // 获取鼠标点击位置
            QPointF pos = event->pos();
            // 判断是否点击到某个顶点
            if (isVertexClicked(pos)) {
                sig1=1;
                vertexClickReaction();
                sig1=0;
            }
            // 鼠标右键点击到边界
            if(isBoundaryClicked(pos))
            {

                boundaryClickReaction(pos);
                pixmapnum++;
                sig2=0;
            }
        }else if(event->button() == Qt::MiddleButton) // 中箭点击添加分布力
        {
            qDebug()<<"middle clicked";

            QPointF pos = event->pos();

            if(previousPos.isNull()) // 第一点为空
            {
                if(isVertexClicked(pos)){
                    previousPos = vertices[sig-1];
                }
                if(isBoundaryClicked(pos)){
                    // 获取离鼠标点击点最近的矩形上的坐标点
                    if(sig2==1){ //
                        previousPos.setX(pos.x());
                        previousPos.setY(m_rect.top());
                    }else if(sig2==2){ //
                        previousPos.setX(pos.x());
                        previousPos.setY(m_rect.bottom());
                    }else if(sig2==3){ //
                        previousPos.setY(pos.y());
                        previousPos.setX(m_rect.left());
                    }else { //
                        previousPos.setY(pos.y());
                        previousPos.setX(m_rect.right());
                    }
                }
            }else{
                if(isVertexClicked(pos)){
                    currentPos = vertices[sig-1];
                }
                if(isBoundaryClicked(pos)){
                    // 获取离鼠标点击点最近的矩形上的坐标点
                    if(sig2==1){ //
                        currentPos.setX(pos.x());
                        currentPos.setY(m_rect.top());
                    }else if(sig2==2){ //
                        currentPos.setX(pos.x());
                        currentPos.setY(m_rect.bottom());
                    }else if(sig2==3){ //
                        currentPos.setY(pos.y());
                        currentPos.setX(m_rect.left());
                    }else { //
                        currentPos.setY(pos.y());
                        currentPos.setX(m_rect.right());
                    }
                }

                if(previousPos.x()==currentPos.x()){ // 点击的点已经被存储
                    if(previousPos.x()==m_rect.left())
                    {
                        drawDistributedForce(previousPos,currentPos,1);
    //                    qDebug()<<"left";
                    }
                    else if(previousPos.x()==m_rect.right())
                    {
                        drawDistributedForce(previousPos,currentPos,2);
    //                    qDebug()<<"right";
                    }
                }else if(previousPos.y()==currentPos.y()){
                    if(previousPos.y()==m_rect.top())
                    {
                        drawDistributedForce(previousPos,currentPos,3);
    //                    qDebug()<<"top";
                    }
                    else if(previousPos.y()==m_rect.bottom())
                    {
                        drawDistributedForce(previousPos,currentPos,4);
    //                    qDebug()<<"bottom";
                    }
                }
                previousPos = QPointF();

            }

        }
    }else{
        if (event->button() == Qt::LeftButton)
        {
            QPointF clickedPoint = event->pos();
            QString str = "";

            // 遍历每个点，检查点击位置是否在其附近的区域内
            for (const myPoint& point : nodes2)
            {
                QRectF clickableArea(point.getX() - 2, point.getY() - 2, 4, 4); // 定义每个点周围的区域

                if (clickableArea.contains(clickedPoint))
                {
                    int k = point.getId();
                    str+="\n编号:";
                    str+=QString::number(k);


                    str+="\n位移(x,y):\n";
                    str+=QString::number(delta[2*k]);
                    str+="\n";
                    str+=QString::number(delta[2*k+1]);

                    updateTextEdit(0,str);

//                    str+="\n应变(x,y,γ):";
//                    str+=QString::number(point.getId());
//                    str+="\n位移(x,y,γ):";
//                    str+=QString::number(point.getId());

                    break;
                }
            }

        }
    }



}


bool MyDrawWidget::isVertexClicked(QPointF& posp)
{
    if(m_rect.isEmpty()||m_rect.isNull())
        return false;

    // 判断是否在顶点附近, sig用于区分在哪个顶点附近, 用于后续图形的绘制.
    if (QRectF(m_rect.topLeft() - QPointF(5, 5), QSizeF(10, 10)).contains(posp))
    {
        sig = 1;
        return true;
    }
    if (QRectF(m_rect.topRight() - QPointF(5, 5), QSizeF(10, 10)).contains(posp))
    {
        sig = 2;
        return true;
    }
    if (QRectF(m_rect.bottomLeft() - QPointF(5, 5), QSizeF(10, 10)).contains(posp))
    {
        sig = 3;
        return true;
    }
    if (QRectF(m_rect.bottomRight() - QPointF(5, 5), QSizeF(10, 10)).contains(posp))
    {
        sig = 4;
        return true;
    }

    qDebug()<<sig;
    return false;
}


void MyDrawWidget::vertexClickReaction()
{
    // 创建弹出窗口
    QDialog* popup = new QDialog(this);
    popup->setWindowTitle("顶点约束");
    popup->setFixedSize(150,100);

    // 创建选项控件
    QCheckBox* option1 = new QCheckBox("水平约束");
    QCheckBox* option2 = new QCheckBox("竖直约束");

    // 创建linedit布局
    QLineEdit* edit1 = new QLineEdit();
    QLineEdit* edit2 = new QLineEdit();



    edit1->setPlaceholderText("水平集中力");
    edit2->setPlaceholderText("竖直集中力");

    bool ok=1; // 用来判断输入
    bool ok2=1;



    // 将矩形四个顶点放入数组
    QPointF vertices[4];
    vertices[0] = m_rect.topLeft();
    vertices[1] = m_rect.topRight();
    vertices[2] = m_rect.bottomLeft();
    vertices[3] = m_rect.bottomRight();





    // 布局选项控件
    QVBoxLayout* layout = new QVBoxLayout(popup);
    if(sig1==0)
    {
        layout->addWidget(option1);
        layout->addWidget(option2);
    }

    // 如果是关于力的, 那么更改控件名称
    if(sig1==1)
    {
        popup->setFixedSize(150,120);
        popup->setWindowTitle("集中力约束");
        layout->addWidget(edit1);
        layout->addWidget(edit2);
    }

    popup->setLayout(layout);

    // 显示弹出窗口
    popup->exec();


    addCons.resize(2*myNodes.size());
    addForce.resize(2*myNodes.size());
    // 处理选项的状态
    bool isOption1Checked = option1->isChecked();
    bool isOption2Checked = option2->isChecked();


    if(sig1==0)
    {

        // TODO: 根据选项的状态进行处理
        if(sig==1)
        {

            drawConstriant(m_rect.topLeft(),isOption1Checked,isOption2Checked);
        }
        if(sig==2)
        {
            drawConstriant(m_rect.topRight(),isOption1Checked,isOption2Checked);
        }
        if(sig==3)
        {
            drawConstriant(m_rect.bottomLeft(),isOption1Checked,isOption2Checked);
        }
        if(sig==4)
        {
            drawConstriant(m_rect.bottomRight(),isOption1Checked,isOption2Checked);
        }
        foreach(auto& E, myNodes){
            if((abs(vertices[sig-1].x()-E.getX())<1e-6) && (abs(vertices[sig-1].y()-E.getY())<1e-6))
            {
                if(isOption1Checked)
                    addCons[2*E.getId()]=1;
                if(isOption2Checked)
                    addCons[2*E.getId()+1]=1;
            }
        }
        qDebug()<<"constriant done";
    }
    if(sig1==1)
    {  // 把力进行存储, 以及对应点
        MyForce f(0,QPointF(0,0),0,0);


        QPair<double, double> values = getLineEditValues(edit1, edit2);
        double value1 = values.first;
        double value2 = values.second;

        // 没有力就不画三角形
        if(value1==0.0)
            ok=0;
        if(value2==0.0)
            ok2=0;
        f.id=sig; // 代表是哪个顶点
        f.position=vertices[sig-1];
        f.sizex=value1;
        f.sizey=value2;
        vertexForce.append(f);


        foreach(auto& E, myNodes){
            if((abs(vertices[sig-1].x()-E.getX())<1e-6) && (abs(vertices[sig-1].y()-E.getY())<1e-6))
            {
                if(ok)
                    addForce[2*E.getId()]=value1;
                if(ok2)
                    addForce[2*E.getId()+1]=value2;
            }
        }



        if(sig==1)
            drawConcentratedForce(m_rect.topLeft(),ok,ok2);
        if(sig==2)
            drawConcentratedForce(m_rect.topRight(),ok,ok2);
        if(sig==3)
            drawConcentratedForce(m_rect.bottomLeft(),ok,ok2);
        if(sig==4)
            drawConcentratedForce(m_rect.bottomRight(),ok,ok2);

        qDebug()<<"Force done";
    }

    for(int i=0;i<addCons.size();i++){
        if(addCons[i])
            qDebug()<<i;
    }
    for(int i=0;i<addForce.size();i++)
    {
        if(addForce[i])
            qDebug()<<i<<addForce[i];
    }
}



// 不包括边界
bool MyDrawWidget::isBoundaryClicked(QPointF& posp)
{

    if(m_rect.isEmpty()||m_rect.isNull())
        return 0;

    QPointF point = posp;
    int tolerance = 5; // 增加点击响应的范围


    // 在边不在点
    if (point.y() >= m_rect.y() - tolerance && point.y() <= m_rect.y() + tolerance
            && point.x() >= m_rect.x()+tolerance && point.x() <= m_rect.x() + m_rect.width()-tolerance) {
        // 点在上边界上且在矩形的 x 范围内
        // 执行相应的操作
        sig2 = 1;
        return 1;
    } else if (point.y() >= m_rect.y() + m_rect.height() - tolerance && point.y() <= m_rect.y() + m_rect.height() + tolerance
               && point.x() >= m_rect.x()+tolerance && point.x() <= m_rect.x() + m_rect.width()-tolerance) {
        // 点在下边界上且在矩形的 x 范围内
        // 执行相应的操作
        sig2 = 2;
        return 1;
    } else if (point.x() >= m_rect.x() - tolerance && point.x() <= m_rect.x() + tolerance
               && point.y() >= m_rect.y()+tolerance && point.y() <= m_rect.y() + m_rect.height()-tolerance) {
        // 点在左边界上且在矩形的 y 范围内
        // 执行相应的操作
        sig2 = 3;
        return 1;
    } else if (point.x() >= m_rect.x() + m_rect.width() - tolerance && point.x() <= m_rect.x() + m_rect.width() + tolerance
               && point.y() >= m_rect.y()+tolerance && point.y() <= m_rect.y() + m_rect.height()-tolerance) {
        // 点在右边界上且在矩形的 y 范围内
        // 执行相应的操作
        sig2 = 4;
        return 1;
    }

    return 0;
}



//
void MyDrawWidget::boundaryClickReaction(QPointF& posp)
{

    QPointF truepoint;
    addForce.resize(2*myNodes.size());

    // 获取离鼠标点击点最近的矩形上的坐标点
    if(sig2==1){ //
        truepoint.setX(posp.x());
        truepoint.setY(m_rect.top());
    }else if(sig2==2){ //
        truepoint.setX(posp.x());
        truepoint.setY(m_rect.bottom());
    }else if(sig2==3){ //
        truepoint.setY(posp.y());
        truepoint.setX(m_rect.left());
    }else if(sig2==4){ //
        truepoint.setY(posp.y());
        truepoint.setX(m_rect.right());
    }


    // 创建弹出窗口
    QDialog* popup = new QDialog(this);
    popup->setWindowTitle("集中力约束");
    popup->setFixedSize(150,50);

    // 创建linedit布局
    QLineEdit* edit1 = new QLineEdit();
    QLineEdit* edit2 = new QLineEdit();
    edit1->setPlaceholderText("水平集中力");
    edit2->setPlaceholderText("竖直集中力");



    // 布局选项控件
    QVBoxLayout* layout = new QVBoxLayout(popup);
    if(sig2==1||sig2==2)
        layout->addWidget(edit2);
    else
        layout->addWidget(edit1);

    // 显示弹出窗口
    popup->setLayout(layout);
    popup->exec();






    double value = 0.0;

    MyForce oneForce(0,QPointF(0,0),0,0);

    if(sig2==1||sig2==2)
    {
        value = getLineEditValues2(edit2);

        if(value)
        {
            oneForce.id = sig2;
            oneForce.position = truepoint;
            oneForce.sizey = value;


            double minDis=100;
            int noMinDis=0;

            foreach(auto& E, myNodes){
                // 遍历点集，查找与力的作用点重合的点
                if((abs(truepoint.x()-E.getX())<1e-6)&&(abs(truepoint.y()-E.getY())<1e-6))
                {
                    addForce[2*E.getId()+1]=value;
                    // qDebug()<<(2*E.getId()+1)<<"123456";
                }else{ // 不存在一个重合的点
                    //查找距离力作用点最近的点
                    if(abs(E.getY()-truepoint.y())<1e-6){

                        double dis = abs(truepoint.x()-E.getX());
                        if(dis<minDis){
                            minDis=dis;
                            noMinDis=E.getId();
                        }

                    }

                }
            }
            addForce[2*noMinDis+1]=value;

        }
    }
    else
    {
        value = getLineEditValues2(edit1);
        if(value)
        {
            oneForce.id = sig2;
            oneForce.position = truepoint;
            oneForce.sizex = value;

            double minDis=100;
            int noMinDis=0;

            foreach(auto& E, myNodes){
                if((abs(truepoint.y()-E.getY())<1e-6)&&(abs(truepoint.y()-E.getY())<1e-6))
                {
                    addForce[2*E.getId()]=value;
                }else{ // 不存在一个重合的点
                    //查找距离力作用点最近的点
                    if(abs(E.getX()-truepoint.x())<1e-6){

                        double dis = abs(truepoint.y()-E.getY());
                        if(dis<minDis){
                            minDis=dis;
                            noMinDis=E.getId();
                        }

                    }

                }

            }
            addForce[2*noMinDis]=value;
        }
    }
    forceInfo.append(oneForce);
    qDebug()<<"---------------------------";
//    for (const auto& element : forceInfo) {
//        qDebug()<<element.id;
//        qDebug()<<element.position;
//        qDebug()<<element.sizex;
//        qDebug()<<element.sizey<<'\n';
//    }
    for(int i=0;i<addForce.size();i++)
    {
        if(addForce[i])
            qDebug()<<i<<addForce[i]<<'\n';
    }

    qDebug()<<"---------------------------";

    QPixmap pixmap(size());
    pixmap.fill(Qt::transparent); // 用透明色填充整个pixmap

    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::red, 2));
    painter.setBrush(Qt::red);

    // 四个箭头的顶点
    QPointF arrowDown2Point(truepoint.x(), truepoint.y() + 15);
    QPointF arrowDownPoint(truepoint.x(), truepoint.y() );
    QPointF arrowRight2Point(truepoint.x() + 15, truepoint.y());
    QPointF arrowRightPoint(truepoint.x() , truepoint.y());

    // 绘制下箭头2
    QPointF arrowDown2Points[4] = {
        arrowDown2Point,
        QPointF(arrowDown2Point.x(),arrowDown2Point.y()-15),
        QPointF(arrowDown2Point.x() - 3, arrowDown2Point.y() - 3),
        QPointF(arrowDown2Point.x() + 3, arrowDown2Point.y() - 3)
    };

    // 绘制下箭头
    QPointF arrowDownPoints[4] = {
        arrowDownPoint,
        QPointF(arrowDownPoint.x(),arrowDownPoint.y()-15),
        QPointF(arrowDownPoint.x() - 3, arrowDownPoint.y() - 3),
        QPointF(arrowDownPoint.x() + 3, arrowDownPoint.y() - 3)
    };

    // 绘制右箭头2
    QPointF arrowRight2Points[4] = {
        arrowRight2Point,
        QPointF(arrowRight2Point.x() - 15,arrowRight2Point.y()),
        QPointF(arrowRight2Point.x() - 3, arrowRight2Point.y() - 3),
        QPointF(arrowRight2Point.x() - 3, arrowRight2Point.y() + 3)
    };

    // 绘制右箭头
    QPointF arrowRightPoints[4] = {
        arrowRightPoint,
        QPointF(arrowRightPoint.x() - 15,arrowRightPoint.y()),
        QPointF(arrowRightPoint.x() - 3, arrowRightPoint.y() - 3),
        QPointF(arrowRightPoint.x() - 3, arrowRightPoint.y() + 3)
    };


    switch(sig2)
    {
    case 1:
        if(value)
        {
            painter.drawLine(arrowDownPoints[0],arrowDownPoints[1]);
            painter.drawLine(arrowDownPoints[0],arrowDownPoints[2]);
            painter.drawLine(arrowDownPoints[0],arrowDownPoints[3]);
        }
        break;
    case 2:
        if(value)
        {
        painter.drawLine(arrowDown2Points[0],arrowDown2Points[1]);
        painter.drawLine(arrowDown2Points[0],arrowDown2Points[2]);
        painter.drawLine(arrowDown2Points[0],arrowDown2Points[3]);
        }

        break;
    case 3:
        if(value)
        {
            painter.drawLine(arrowRightPoints[0], arrowRightPoints[1]);
            painter.drawLine(arrowRightPoints[0], arrowRightPoints[2]);
            painter.drawLine(arrowRightPoints[0], arrowRightPoints[3]);
        }

        break;
    case 4:
        if(value)
        {
            painter.drawLine(arrowRight2Points[0], arrowRight2Points[1]);
            painter.drawLine(arrowRight2Points[0], arrowRight2Points[2]);
            painter.drawLine(arrowRight2Points[0], arrowRight2Points[3]);
        }

        break;
    default:
        break;
    }
    pixmapArray[pixmapnum] = pixmap;
    update();


    qDebug()<<"force done";
}


// 绘制四个顶点的位移约束
// 矩形为
void MyDrawWidget::drawConstriant(const QPointF point, bool s1, bool s2)
{
    QPixmap pixmap(size());
    pixmap.fill(Qt::transparent); // 用透明色填充整个pixmap

    QPainter painter(&pixmap);
    painter.setPen(Qt::black);
    painter.setBrush(Qt::black);
    switch(sig) // 用于对不同情况做不同判断
    {
    case 1:
        if(s1==1)
        {
            painter.setBrush(Qt::black);

            // 绘制水平方向的二力杆
            QRectF horizontalRect(point.x() - 18, point.y() - 1, 10, 2); // 二力杆的位置和大小
            painter.drawRect(horizontalRect); // 绘制矩形
            painter.setBrush(Qt::NoBrush);
            // 绘制两个小圆
            QRectF leftCircle(point.x() - 26, point.y() - 4, 8, 8); // 左边小圆的位置和大小
            QRectF rightCircle(point.x() - 8, point.y() - 4, 8, 8); // 右边小圆的位置和大小
            painter.drawEllipse(leftCircle); // 绘制左边小圆
            painter.drawEllipse(rightCircle); // 绘制右边小圆

        }
        if(s2==1)
        {
            painter.setBrush(Qt::black);

            // 绘制垂直方向的二力杆
            QRectF verticalRect(point.x() - 1, point.y() - 18, 2, 10); // 二力杆的位置和大小
            painter.drawRect(verticalRect); // 绘制矩形
            painter.setBrush(Qt::NoBrush);
            // 绘制两个小圆
            QRectF topCircle(point.x() - 4, point.y() - 26, 8, 8); // 上边小圆的位置和大小
            QRectF bottomCircle(point.x() - 4, point.y() - 8, 8, 8); // 下边小圆的位置和大小
            painter.drawEllipse(topCircle); // 绘制上边小圆
            painter.drawEllipse(bottomCircle); // 绘制下边小圆
        }
        pixmapArray[4] = pixmap;
        break;
    case 2:
        if(s1==1)
        {
            painter.setBrush(Qt::black);

            // 绘制水平方向的二力杆
            QRectF horizontalRect(point.x() + 8, point.y() - 1, 10, 2); // 二力杆的位置和大小
            painter.drawRect(horizontalRect); // 绘制矩形
            painter.setBrush(Qt::NoBrush);
            // 绘制两个小圆
            QRectF leftCircle(point.x(), point.y() - 4, 8, 8); // 左边小圆的位置和大小
            QRectF rightCircle(point.x() + 18, point.y() - 4, 8, 8); // 右边小圆的位置和大小
            painter.drawEllipse(leftCircle); // 绘制左边小圆
            painter.drawEllipse(rightCircle); // 绘制右边小圆

        }
        if(s2==1)
        {
            painter.setBrush(Qt::black);

            // 绘制垂直方向的二力杆
            QRectF verticalRect(point.x() - 1, point.y() - 18, 2, 10); // 二力杆的位置和大小
            painter.drawRect(verticalRect); // 绘制矩形
            painter.setBrush(Qt::NoBrush);
            // 绘制两个小圆
            QRectF topCircle(point.x() - 4, point.y() - 26, 8, 8); // 上边小圆的位置和大小
            QRectF bottomCircle(point.x() - 4, point.y() - 8, 8, 8); // 下边小圆的位置和大小
            painter.drawEllipse(topCircle); // 绘制上边小圆
            painter.drawEllipse(bottomCircle); // 绘制下边小圆
        }
        pixmapArray[5] = pixmap;
        break;
    case 3:
        if(s1==1)
        {
            painter.setBrush(Qt::black);

            // 绘制水平方向的二力杆
            QRectF horizontalRect(point.x() - 18, point.y() - 1, 10, 2); // 二力杆的位置和大小
            painter.drawRect(horizontalRect); // 绘制矩形
            painter.setBrush(Qt::NoBrush);
            // 绘制两个小圆
            QRectF leftCircle(point.x() - 26, point.y() - 4, 8, 8); // 左边小圆的位置和大小
            QRectF rightCircle(point.x() - 8, point.y() - 4, 8, 8); // 右边小圆的位置和大小
            painter.drawEllipse(leftCircle); // 绘制左边小圆
            painter.drawEllipse(rightCircle); // 绘制右边小圆

        }
        if(s2==1)
        {
            painter.setBrush(Qt::black);

            // 绘制垂直方向的二力杆
            QRectF verticalRect(point.x() - 1, point.y() + 8, 2, 10); // 二力杆的位置和大小
            painter.drawRect(verticalRect); // 绘制矩形
            painter.setBrush(Qt::NoBrush);
            // 绘制两个小圆
            QRectF topCircle(point.x() - 4, point.y(), 8, 8); // 上边小圆的位置和大小
            QRectF bottomCircle(point.x() - 4, point.y() + 18, 8, 8); // 下边小圆的位置和大小
            painter.drawEllipse(topCircle); // 绘制上边小圆
            painter.drawEllipse(bottomCircle); // 绘制下边小圆
        }
        pixmapArray[6] = pixmap;
        break;
    case 4:
        if(s1==1)
        {
            painter.setBrush(Qt::black);

            // 绘制水平方向的二力杆
            QRectF horizontalRect(point.x() + 8, point.y() - 1, 10, 2); // 二力杆的位置和大小
            painter.drawRect(horizontalRect); // 绘制矩形
            painter.setBrush(Qt::NoBrush);
            // 绘制两个小圆
            QRectF leftCircle(point.x(), point.y() - 4, 8, 8); // 左边小圆的位置和大小
            QRectF rightCircle(point.x() + 18, point.y() - 4, 8, 8); // 右边小圆的位置和大小
            painter.drawEllipse(leftCircle); // 绘制左边小圆
            painter.drawEllipse(rightCircle); // 绘制右边小圆

        }
        if(s2==1)
        {
            painter.setBrush(Qt::black);

            // 绘制垂直方向的二力杆
            QRectF verticalRect(point.x() - 1, point.y() + 8, 2, 10); // 二力杆的位置和大小
            painter.drawRect(verticalRect); // 绘制矩形
            painter.setBrush(Qt::NoBrush);
            // 绘制两个小圆
            QRectF topCircle(point.x() - 4, point.y(), 8, 8); // 上边小圆的位置和大小
            QRectF bottomCircle(point.x() - 4, point.y() + 18, 8, 8); // 下边小圆的位置和大小
            painter.drawEllipse(topCircle); // 绘制上边小圆
            painter.drawEllipse(bottomCircle); // 绘制下边小圆
        }
        pixmapArray[7] = pixmap;
        break;
    default:
        break;

    }

    update();

}



//
void MyDrawWidget::drawConcentratedForce(const QPointF point, bool s1, bool s2) // 集中约束力绘制
{
    QPixmap pixmap(size());
    pixmap.fill(Qt::transparent); // 用透明色填充整个pixmap

    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::red, 2));
    painter.setBrush(Qt::red);

    // 四个箭头的顶点
    QPointF arrowDown2Point(point.x(), point.y() + 15);
    QPointF arrowDownPoint(point.x(), point.y() );
    QPointF arrowRight2Point(point.x() + 15, point.y());
    QPointF arrowRightPoint(point.x() , point.y());

    // 绘制下箭头2
    QPointF arrowDown2Points[4] = {
        arrowDown2Point,
        QPointF(arrowDown2Point.x(),arrowDown2Point.y()-15),
        QPointF(arrowDown2Point.x() - 3, arrowDown2Point.y() - 3),
        QPointF(arrowDown2Point.x() + 3, arrowDown2Point.y() - 3)
    };

    // 绘制下箭头
    QPointF arrowDownPoints[4] = {
        arrowDownPoint,
        QPointF(arrowDownPoint.x(),arrowDownPoint.y()-15),
        QPointF(arrowDownPoint.x() - 3, arrowDownPoint.y() - 3),
        QPointF(arrowDownPoint.x() + 3, arrowDownPoint.y() - 3)
    };

    // 绘制右箭头2
    QPointF arrowRight2Points[4] = {
        arrowRight2Point,
        QPointF(arrowRight2Point.x() - 15,arrowRight2Point.y()),
        QPointF(arrowRight2Point.x() - 3, arrowRight2Point.y() - 3),
        QPointF(arrowRight2Point.x() - 3, arrowRight2Point.y() + 3)
    };

    // 绘制右箭头
    QPointF arrowRightPoints[4] = {
        arrowRightPoint,
        QPointF(arrowRightPoint.x() - 15,arrowRightPoint.y()),
        QPointF(arrowRightPoint.x() - 3, arrowRightPoint.y() - 3),
        QPointF(arrowRightPoint.x() - 3, arrowRightPoint.y() + 3)
    };
    if(sig1==1)
    {
        switch(sig)
        {
        case 1:
            if(s1){
                painter.drawLine(arrowRightPoints[0], arrowRightPoints[1]);
                painter.drawLine(arrowRightPoints[0], arrowRightPoints[2]);
                painter.drawLine(arrowRightPoints[0], arrowRightPoints[3]);
            }
            if(s2){
                painter.drawLine(arrowDownPoints[0],arrowDownPoints[1]);
                painter.drawLine(arrowDownPoints[0],arrowDownPoints[2]);
                painter.drawLine(arrowDownPoints[0],arrowDownPoints[3]);
            }
            pixmapArray[8] = pixmap;
            break;
        case 2:
            if(s1){
                painter.drawLine(arrowRight2Points[0], arrowRight2Points[1]);
                painter.drawLine(arrowRight2Points[0], arrowRight2Points[2]);
                painter.drawLine(arrowRight2Points[0], arrowRight2Points[3]);
            }
            if(s2){
                painter.drawLine(arrowDownPoints[0],arrowDownPoints[1]);
                painter.drawLine(arrowDownPoints[0],arrowDownPoints[2]);
                painter.drawLine(arrowDownPoints[0],arrowDownPoints[3]);
            }
            pixmapArray[9] = pixmap;
            break;
        case 3:
            if(s1){
                painter.drawLine(arrowRightPoints[0], arrowRightPoints[1]);
                painter.drawLine(arrowRightPoints[0], arrowRightPoints[2]);
                painter.drawLine(arrowRightPoints[0], arrowRightPoints[3]);
            }
            if(s2){
                painter.drawLine(arrowDown2Points[0],arrowDown2Points[1]);
                painter.drawLine(arrowDown2Points[0],arrowDown2Points[2]);
                painter.drawLine(arrowDown2Points[0],arrowDown2Points[3]);
            }
            pixmapArray[10] = pixmap;
            break;
        case 4:
            if(s1){
                painter.drawLine(arrowRight2Points[0], arrowRight2Points[1]);
                painter.drawLine(arrowRight2Points[0], arrowRight2Points[2]);
                painter.drawLine(arrowRight2Points[0], arrowRight2Points[3]);
            }
            if(s2){
                painter.drawLine(arrowDown2Points[0],arrowDown2Points[1]);
                painter.drawLine(arrowDown2Points[0],arrowDown2Points[2]);
                painter.drawLine(arrowDown2Points[0],arrowDown2Points[3]);
            }
            pixmapArray[11] = pixmap;
            break;
        default :
            break;
        }
    }
    update();
}


// 获取弹出窗口中的内容
QPair<double, double> MyDrawWidget::getLineEditValues(QLineEdit *lineEdit1, QLineEdit *lineEdit2)
{

    // 获取LineEdit的内容
    QString input1 = lineEdit1->text();
    QString input2 = lineEdit2->text();

    double value1=0.0;
    double value2=0.0;


    // 正则表达 判断是否为浮点
    static QRegularExpression floatRegex("^[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?$");


    if (floatRegex.match(input1).hasMatch())
    {

        value1 = input1.toDouble();
        qDebug() << "Input 1 is a float";
    }
    else
    {
        qDebug() << "Input 1 is not a float";
    }

    if (floatRegex.match(input2).hasMatch())
    {
        value2 = input2.toDouble();
        qDebug() << "Input 2 is a float";
    }
    else
    {
        qDebug() << "Input 2 is not a float";
    }


    return qMakePair(value1, value2);
}



double MyDrawWidget::getLineEditValues2(QLineEdit* lineEdit)
{
    // 获取LineEdit的内容
    QString input = lineEdit->text();

    double value=0.0;


    // 正则表达 判断是否为浮点
    static QRegularExpression floatRegex("^[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?$");
    if (floatRegex.match(input).hasMatch())
    {

        value = input.toDouble();
        qDebug() << "Input is a float";
    }
    else
    {
        qDebug() << "Input is not a float";
    }

    return value;

}



void MyDrawWidget::drawDistributedForce(const QPointF point1, const QPointF point2, int t) // 分布约束力绘制
{
    // 创建弹出窗口
    QDialog* popup = new QDialog(this);
    popup->setWindowTitle("均布载荷");
    popup->setFixedSize(150,50);

    // 创建linedit布局
    QLineEdit* edit1 = new QLineEdit();
    QLineEdit* edit2 = new QLineEdit();
    edit1->setPlaceholderText("水平分布力");
    edit2->setPlaceholderText("竖直分布力");



    // 布局选项控件
    QVBoxLayout* layout = new QVBoxLayout(popup);
    if(t==1||t==2)
        layout->addWidget(edit1);
    else
        layout->addWidget(edit2);

    // 显示弹出窗口
    popup->setLayout(layout);
    popup->exec();
    double value = 0.0;

    MyForce oneForce(0,QPointF(0,0),0,0);
    MyForce twoForce(0,QPointF(0,0),0,0);



    if(t==1||t==2)
    {
        value = getLineEditValues2(edit1);
        if(value)
        {
            QVector<int> No;
            int tempno=0;
            double Fx = abs(point1.y()-point2.y())*value; //合力
            foreach(auto& e,myNodes){
                if(abs(e.getX()-point1.x())<1e-10 && (point1.y()-e.getY())*(point2.y()-e.getY())<=0){
                    tempno++;
                    No.append(e.getId());
                }
            }
            for(int k=0;k<tempno;k++)
                addForce[2*No[k]]=1.0*Fx/tempno;

        }else{
            return;
        }
    }
    else
    {
        value = getLineEditValues2(edit2);
        if(value)
        {
            QVector<int> No;
            int tempno=0;
            double Fx = abs(point1.x()-point2.x())*value; //合力
            foreach(auto& e,myNodes){
                if(abs(e.getY()-point1.y())<1e-10 && (point1.x()-e.getX())*(point2.x()-e.getX())<=0){
                    tempno++;
                    No.append(e.getId());
                }
            }
            for(int k=0;k<tempno;k++)
                addForce[2*No[k]+1]=1.0*Fx/tempno;
        }else{
            return;
        }
    }

    for(int i=0;i<addForce.size();i++){
        if(addForce[i]!=0){
            qDebug()<<i<<addForce[i];
        }
    }

    QPixmap pixmap(size());
    pixmap.fill(Qt::transparent); // 用透明色填充整个pixmap

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(Qt::green, 0.5));
    painter.setBrush(QBrush(Qt::green));

    int spacing = 10;  // 相邻两条线的间距
    int length = 15;   // 线的长度

    // 1 左边 2 右边 3 上边 4 下边
    if(t==3){
        QPointF drawpoint(0,0);
        if(point1.x()<point2.x()){
            drawpoint=point1;
        }else{
            drawpoint=point2;
        }
        for(int i=0;i<abs(point1.x()-point2.x())/spacing+1;i++){
            painter.drawLine(drawpoint.x()+i*spacing,drawpoint.y(),drawpoint.x()+i*spacing,drawpoint.y()-length);
            painter.drawLine(drawpoint.x()+i*spacing,drawpoint.y(),drawpoint.x()+i*spacing-3,drawpoint.y()-3);
            painter.drawLine(drawpoint.x()+i*spacing,drawpoint.y(),drawpoint.x()+i*spacing+3,drawpoint.y()-3);
            pixmapArray[12]=pixmap;
        }
    }
    if(t==4){
        QPointF drawpoint(0,0);
        if(point1.x()<point2.x()){
            drawpoint=point1;
        }else{
            drawpoint=point2;
        }
        for(int i=0;i<abs(point1.x()-point2.x())/spacing+1;i++){
            painter.drawLine(drawpoint.x()+i*spacing,drawpoint.y(),drawpoint.x()+i*spacing,drawpoint.y()+length);
            painter.drawLine(drawpoint.x()+i*spacing,drawpoint.y()+length,drawpoint.x()+i*spacing-3,drawpoint.y()+length-3);
            painter.drawLine(drawpoint.x()+i*spacing,drawpoint.y()+length,drawpoint.x()+i*spacing+3,drawpoint.y()+length-3);
            pixmapArray[13]=pixmap;
        }
    }
    if(t==1){
        QPointF drawpoint(0,0);
        if(point1.y()<point2.y()){
            drawpoint=point1;
        }else{
            drawpoint=point2;
        }
        for(int i=0;i<abs(point1.y()-point2.y())/spacing+1;i++){
            painter.drawLine(drawpoint.x()-length,drawpoint.y()+i*spacing,drawpoint.x(),drawpoint.y()+i*spacing);
            painter.drawLine(drawpoint.x(),drawpoint.y()+i*spacing,drawpoint.x()-3,drawpoint.y()+i*spacing+3);
            painter.drawLine(drawpoint.x(),drawpoint.y()+i*spacing,drawpoint.x()-3,drawpoint.y()+i*spacing-3);
            pixmapArray[14]=pixmap;
        }
    }
    if(t==2){
        QPointF drawpoint(0,0);
        if(point1.y()<point2.y()){
            drawpoint=point1;
        }else{
            drawpoint=point2;
        }
        for(int i=0;i<abs(point1.y()-point2.y())/spacing+1;i++){
            painter.drawLine(drawpoint.x(),drawpoint.y()+i*spacing,drawpoint.x()+length,drawpoint.y()+i*spacing);
            painter.drawLine(drawpoint.x()+length,drawpoint.y()+i*spacing,drawpoint.x()+length-3,drawpoint.y()+i*spacing+3);
            painter.drawLine(drawpoint.x()+length,drawpoint.y()+i*spacing,drawpoint.x()+length-3,drawpoint.y()+i*spacing-3);
            pixmapArray[15]=pixmap;
        }
    }
}


void MyDrawWidget::calcMatrix(){
    if(myNodes.isEmpty())
    {
        emit updateTextEdit(0, "not found.");

        return;
    }

    int sign = 2*myNodes.size();

    QVector<QVector<QVector<double>>>matrix;

    for(int i=0;i<myElem.size();i++){
        matrix.push_back(createElementMatrix(myElem[i],myNodes,m_material));

    }

    ALLMatrix = createKMatrix(matrix,myNodes.size(),myElem.size(),myElem);


    // 对角元素改1
    for(int i=0;i<sign;i++)
    {
        if(addCons[i]==1)
        {
            for(int k=0;k<sign;k++)
            {
                ALLMatrix[i][k]=0;
                ALLMatrix[k][i]=0;
            }
            ALLMatrix[i][i]=1;
        }
    }

    for(int i=0;i<myNodes.size();i++){
        if(!m_center.isNull()){
            if(!m_center2.isNull()){
                if((myNodes[i].getX()==m_center.x()&&myNodes[i].getY()==m_center.y()) || (myNodes[i].getX()==m_center2.x()&&myNodes[i].getY()==m_center2.y()))
                {
                    ALLMatrix[2*i][2*i]=1;
                    ALLMatrix[2*i+1][2*i+1]=1;
                }

            }else{
                if(myNodes[i].getX()==m_center.x()&&myNodes[i].getY()==m_center.y())
                {
                    ALLMatrix[2*i][2*i]=1;
                    ALLMatrix[2*i+1][2*i+1]=1;
                }
            }
        }
    }



    QVector<QVector<double>> calcmatrix(sign, QVector<double>(sign + 1, 0));


    // 将力和总刚放在一起
    for (int i = 0; i < sign; i++)
    {
        for (int j = 0; j < sign; j++)
        {
            calcmatrix[i][j] = ALLMatrix[i][j];
        }
    }
    qDebug()<<"5555";
    for (int i = 0; i < sign; i++)
    {
        calcmatrix[i][sign] = addForce[i];
    }
    qDebug()<<"6666";
    double **K4;
    K4 = new double* [sign + 1];
    for (int i = 0; i < sign + 1; i++)
            K4[i] = new double[sign + 2];
    for (int i = 0; i < sign + 1; i++)
            for (int j = 0; j < sign + 2; j++)
                K4[i][j] = 0;
    for (int i = 1; i < sign + 1; i++) {
        K4[i][sign + 1] = addForce[i - 1];
        for (int j = 1; j < sign + 1; j++) {
            K4[i][j] = ALLMatrix[i - 1][j - 1];
        }
    }
    Gauss2_0(K4,sign);


    delta.resize(sign,0);


    for (int i = 1; i <= sign; i++) { // 每个点两个方向位移
        delta[i - 1] = K4[i][sign + 1];
        qDebug() << "delta[" << i-1 << "]" << "=" << delta[i - 1];
    }


    // 计算每个单元应变

    strain.resize(myElem.size());

    for(int i=0;i<myElem.size();i++){
        strain[i].resize(3,0);
    }


    //
    for(int i=0; i<myElem.size();i++){
        double bi=bijk(myNodes[myElem[i].getNode2()].getY(),myNodes[myElem[i].getNode3()].getY());
        double bj=bijk(myNodes[myElem[i].getNode3()].getY(),myNodes[myElem[i].getNode1()].getY());
        double bk=bijk(myNodes[myElem[i].getNode1()].getY(),myNodes[myElem[i].getNode2()].getY());
        double ci=cijk(myNodes[myElem[i].getNode2()].getX(),myNodes[myElem[i].getNode3()].getX());
        double cj=cijk(myNodes[myElem[i].getNode3()].getX(),myNodes[myElem[i].getNode1()].getX());
        double ck=cijk(myNodes[myElem[i].getNode1()].getX(),myNodes[myElem[i].getNode2()].getX());

        // 得到每一个单元应变
        strain[i][0]=bi*delta[2*myElem[i].getNode1()]+bj*delta[2*myElem[i].getNode2()]+bk*delta[2*myElem[i].getNode3()];
        strain[i][1]=ci*delta[2*myElem[i].getNode1()+1]+cj*delta[2*myElem[i].getNode2()+1]+ck*delta[2*myElem[i].getNode3()+1];
        strain[i][2]=bi*delta[2*myElem[i].getNode1()+1]+bj*delta[2*myElem[i].getNode2()+1]+bk*delta[2*myElem[i].getNode3()+1]
                +ci*delta[2*myElem[i].getNode1()]+cj*delta[2*myElem[i].getNode2()]+ck*delta[2*myElem[i].getNode3()];

    }


    // 计算每个单元的应力
    stress.resize(myElem.size());

    for(int i=0;i<myElem.size();i++){
        stress[i].resize(3,0);
    }

    double tempv = m_material.getNu();
    double temp = m_material.getE()/(1-tempv*tempv);

    for(int i=0;i<myElem.size();i++){
        stress[i][0]=temp*(strain[i][0]+tempv*strain[i][1]);
        stress[i][1]=temp*(tempv*strain[i][0]+strain[i][1]);
        stress[i][2]=temp*(strain[i][2]*(1-tempv)/2.0);
    }


    nodes2.resize(myNodes.size());
    //发生位移过后的点的位置
    for(int i=0;i<myNodes.size();i++){
        nodes2[i].setId(myNodes[i].getId());
        nodes2[i].setX(myNodes[i].getX()+delta[2*i]);
        nodes2[i].setY(myNodes[i].getY()+delta[2*i+1]);
    }


    // 获得应力应变的最值
    // 应变
    min[0] = strain[0][0];
    max[0] = strain[0][0];
    for(int i=0;i<strain.size();i++){
        if(strain[i][0]<min[0])
            min[0]=strain[i][0];
        if(strain[i][0]>max[0])
            max[0]=strain[i][0];
    }

    min[1] = strain[0][1];
    max[1] = strain[0][1];
    for(int i=0;i<strain.size();i++){
        if(strain[i][1]<min[1])
            min[1]=strain[i][1];
        if(strain[i][1]>max[1])
            max[1]=strain[i][1];
    }

    min[2] = strain[0][2];
    max[2] = strain[0][2];
    for(int i=0;i<strain.size();i++){
        if(strain[i][2]<min[2])
            min[2]=strain[i][2];
        if(strain[i][2]>max[2])
            max[2]=strain[i][2];
    }

    // 应力
    min[3] = stress[0][0];
    max[3] = stress[0][0];
    for(int i=0;i<stress.size();i++){
        if(stress[i][0]<min[3])
            min[3]=stress[i][0];
        if(stress[i][0]>max[3])
            max[3]=stress[i][0];
    }

    min[4] = stress[0][1];
    max[4] = stress[0][1];
    for(int i=0;i<stress.size();i++){
        if(stress[i][1]<min[4])
            min[4]=stress[i][1];
        if(stress[i][1]>max[4])
            max[4]=stress[i][1];
    }

    min[5] = stress[0][2];
    max[5] = stress[0][2];
    for(int i=0;i<stress.size();i++){
        if(stress[i][2]<min[5])
            min[5]=stress[i][2];
        if(stress[i][2]>max[5])
            max[5]=stress[i][2];
    }


    for (int i = 0; i < sign+1; i++)
            delete[]K4[i];
    delete[]K4;

    beforecalc=0;

    updateTextEdit(0,"计算完成");

}


void MyDrawWidget::drawDisplacement(){

    if(delta.isEmpty())
    {
        emit updateTextEdit(0, "delta not found.");

        return;
    }

    QPixmap pixmap(size());
    pixmap.fill(Qt::transparent); // 用透明色填充整个pixmap

    QPainter painter(&pixmap);
    // 绘制节点
    painter.setPen(Qt::blue);
    painter.setBrush(Qt::blue);
    for (const myPoint& node : nodes2) {
        // 根据节点的位置绘制一个小圆点
        painter.drawEllipse(node.getX() - 1, node.getY() - 1, 2, 2);
    }

    // 绘制三角形单元
    painter.setPen(Qt::black);
    painter.setBrush(Qt::NoBrush);
    for (const myTriangle& triangle : myElem) {
        // 获取三角形单元的节点索引
        int index1 = triangle.getNode1();
        int index2 = triangle.getNode2();
        int index3 = triangle.getNode3();

        // 获取对应的节点坐标
        QPointF point1 = nodes2[index1].toPointF();
        QPointF point2 = nodes2[index2].toPointF();
        QPointF point3 = nodes2[index3].toPointF();

        // 绘制三角形
        QPolygonF polygon;
        polygon << point1 << point2 << point3;
        painter.drawPolygon(polygon);
    }
    clearPixmap();
    pixmapArray[99] = pixmap;


    update();
}


void MyDrawWidget::drawstrain(int k){


    if(strain.isEmpty())
    {
        emit updateTextEdit(0, "strain not found.");

        return;
    }

    // 0 代表水平应变
    // 1 代表竖直应变
    double min1 = min[k];
    double max1 = max[k];



    QPixmap pixmap(size());
    pixmap.fill(Qt::transparent); // 用透明色填充整个pixmap

    QPainter painter(&pixmap);
    // 绘制节点
    painter.setPen(Qt::blue);
    for(int i=0;i<strain.size();i++){
        double value = strain[i][k];
        painter.setBrush(getColorForValue(value,min1,max1));



        // 获取三角形单元的节点索引
        int index1 = myElem[i].getNode1();
        int index2 = myElem[i].getNode2();
        int index3 = myElem[i].getNode3();

        // 获取对应的节点坐标
        QPointF point1 = nodes2[index1].toPointF();
        QPointF point2 = nodes2[index2].toPointF();
        QPointF point3 = nodes2[index3].toPointF();

        // 绘制三角形
        QPolygonF polygon;
        polygon << point1 << point2 << point3;
        painter.drawPolygon(polygon);

    }
    clearPixmap();

    drawAnswer(min1,max1);
    pixmapArray[98] = pixmap;
    update();

}


void MyDrawWidget::drawStress(int k){

    if(stress.isEmpty())
    {
        emit updateTextEdit(0, "stress not found.");

        return;
    }
    // 0 代表水平应力
    // 1 代表竖直应力
    k=k+3;
    double min1 = min[k];
    double max1 = max[k];

    QPixmap pixmap(size());
    pixmap.fill(Qt::transparent); // 用透明色填充整个pixmap

    QPainter painter(&pixmap);
    // 绘制节点
    painter.setPen(Qt::blue);
    for(int i=0;i<strain.size();i++){
        double value = stress[i][k-3];
        painter.setBrush(getColorForValue(value,min1,max1));

        // 获取三角形单元的节点索引
        int index1 = myElem[i].getNode1();
        int index2 = myElem[i].getNode2();
        int index3 = myElem[i].getNode3();

        // 获取对应的节点坐标
        QPointF point1 = nodes2[index1].toPointF();
        QPointF point2 = nodes2[index2].toPointF();
        QPointF point3 = nodes2[index3].toPointF();

        // 绘制三角形
        QPolygonF polygon;
        polygon << point1 << point2 << point3;
        painter.drawPolygon(polygon);

    }

    clearPixmap();
    drawAnswer(min1,max1);

    pixmapArray[97] = pixmap;
    update();

}



//
QColor MyDrawWidget::getColorForValue(double value, double minValue, double maxValue){
    // 计算value在[minValue, maxValue]范围内的比例
    double ratio = (value - minValue) / (maxValue - minValue);

    // 根据相对位置计算RGB值
    int r, g, b;
    if (ratio <= 0.25) {
        b = 255;
        g = (int)(255 * 4 * ratio);
        r = 0;
    } else if (ratio <= 0.5) {
        r = 0;
        g = 255;
        b = (int)(255 *(1-4*(ratio - 0.25)));
    } else if (ratio <= 0.75) {
        r = (int)(255 * (4 * (ratio - 0.5)));
        g = 255;
        b = 0;
    } else {
        r = 255;
        g = (int)(255 * (1-4*(ratio-0.75)));
        b = 0;
    }

    // 创建颜色并返回
    return QColor(r, g, b);
}



void MyDrawWidget::drawAnswer(double minValue, double maxValue){
    // 色带矩形的大小和位置
    QString min = QString::number(minValue);
    QString max = QString::number(maxValue);
    QRectF gradientRect = QRectF(510, 10, 20, 200);

    // 创建一个线性渐变对象
    QLinearGradient gradient(gradientRect.topLeft(), gradientRect.bottomRight());
    gradient.setColorAt(0.0, Qt::blue);   // 设置渐变起始颜色为蓝色
    gradient.setColorAt(0.3, Qt::green);
    gradient.setColorAt(0.6, Qt::yellow);
    gradient.setColorAt(1.0, Qt::red);

    // 创建一个QPainter对象，并设置渐变色
    QPixmap pixmap(size());
    pixmap.fill(Qt::transparent); // 用透明色填充整个pixmap
    QPainter painter(&pixmap);

    painter.setPen(Qt::blue);
    painter.setBrush(QBrush(gradient));
    qDebug() << "here I am";
    // 用QPainter对象的drawRect方法绘制一个矩形
    painter.drawRect(gradientRect);
    painter.setPen(Qt::black);

    painter.drawText(gradientRect.topLeft() + QPoint(-10, 0), min);  // 在矩形的右上方偏移一定距离绘制文字
    painter.drawText(gradientRect.bottomLeft() + QPoint(-10, 10), max);  // 在矩形的右上方偏移一定距离绘制文字

    pixmapArray[96]=pixmap;

}


MyDrawWidget::~MyDrawWidget()
{

}
