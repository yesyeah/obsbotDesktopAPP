#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init(std::shared_ptr<CameraControl> camera){
    ItemModel = new QStandardItemModel(this);
    handle = camera;

    if (handle->CameraAutoZoomGet()){
        ui->zoomAddButton->setEnabled(false);
        ui->zoomReduceButton->setEnabled(false);
        ui->zoomButton->setEnabled(false);
        ui->autoZoomButton->setText("打开");
        ui->autoZoomButton->setIcon(QIcon(":/image/icon/on2.png"));

    } else {
        ui->zoomAddButton->setEnabled(true);
        ui->zoomReduceButton->setEnabled(true);
        ui->zoomButton->setEnabled(true);
        ui->autoZoomButton->setText("关闭");
        ui->autoZoomButton->setIcon(QIcon(":/image/icon/off2.png"));
    }

    if (handle->AITrackingGet()){
        ui->autoTrackingButton->setText("打开");
        ui->autoTrackingButton->setIcon(QIcon(":/image/icon/on2.png"));
        ui->pushButton->setEnabled(true);
        std::cout<<"auto tracking is  on "<<std::endl;
    } else {
        ui->autoTrackingButton->setText("关闭");
        ui->autoTrackingButton->setIcon(QIcon(":/image/icon/off2.png"));
        ui->pushButton->setEnabled(false);
        std::cout<<"auto tracking is  off "<<std::endl;
    }
    if (handle->AIHandposeGet()){
        ui->handPoseButton->setIcon(QIcon(":/image/icon/on2.png"));
        ui->handPoseButton->setText("打开");

    } else {
        ui->handPoseButton->setIcon(QIcon(":/image/icon/off2.png"));
        ui->handPoseButton->setText("关闭");
    }

    if (!handle->AIDefaultViewGet()){
        //default view landscape
        ui->objectDefaultViewButton->setText("横屏");
        std::cout<<"default view  is  landscape "<<std::endl;

    } else {
        std::cout<<"default view is not landscape "<<std::endl;
        ui->objectDefaultViewButton->setText("竖屏");
    }

    ui->zoomAddButton->setDisabled(true);
    ui->zoomAddButton->setVisible(false);
    ui->zoomReduceButton->setDisabled(true);
    ui->zoomReduceButton->setVisible(false);
    ui->zoomButton->setDisabled(true);
    ui->zoomButton->setVisible(false);

    currentZoomValue = handle->GetCurrentZoom();

    ui->zoomLabel_2->setText(QString::fromStdString(std::to_string(currentZoomValue)+"x"));

    ui->zoomSlider->setMinimum(0);
    ui->zoomSlider->setMaximum(8);
    ui->zoomSlider->setSingleStep(1);
    ui->zoomSlider->setTickPosition(QSlider::TicksBelow);
    ui->zoomSlider->setTickInterval(1);
    ui->zoomSlider->setValue(currentZoomValue/0.5 -2);

    //ui->exposeComboBox->setEditable(false);

    // showPresetLocation();
}

void MainWindow::setCommandHandle(std::shared_ptr<CameraControl> camera){
    handle = camera;
    return;
}

void MainWindow::on_presetLocationAddButton_clicked()
{
    if (handle == nullptr){
        std::cout<< "camera ctl handle is null"<<std::endl;
    }
    std::shared_ptr<GimbalLocation> location = handle->CameraDirectionGet();
    std::cout<<"roll:" << location->roll <<" pitch:"<<location->pitch <<" yaw:"<<location->yaw<<std::endl;

    std::string zoom = handle->GetAbsZoom();
    std::cout<<"zoom:" << zoom<<std::endl;

    zoom = zoom.substr(0, zoom.find("x"));
    std::cout<<"zoom:" << zoom<<std::endl;

    handle->GimbalPresetLocationAdd((float)atof(zoom.c_str()), location->roll, location->pitch, location->yaw);

    std::string presetLocationItem = "ratio:" + zoom + "     " +
                                     "roll:" + std::to_string(location->roll) + "     " +
                                     "pitch:" + std::to_string(location->pitch) + "     " +
                                     "yaw:" + std::to_string(location->yaw) + "     ";
    std::cout<<"presetLocationgItem:" << presetLocationItem<<std::endl;
    QString presetLocationStr = QString::fromStdString(presetLocationItem);

    QStringList presetLocationList;
    presetLocationList.append(presetLocationStr);

    QStandardItem *item = new QStandardItem(presetLocationStr);
    ItemModel->appendRow(item);
    ui->presetLocationValueList->setModel(ItemModel);
    //ui->presetLocationValueList->setFixedWidth(311);

    connect(ui->presetLocationValueList, SIGNAL(clicked(QModelIndex)), this, SLOT(triggerPresetLocation(QModelIndex)));
    //ui->presetLocationValueList->clicked.connect(triggerPresetLocation);
}

void MainWindow::triggerPresetLocation(QModelIndex id){
    std::cout<<"trigger a location:"<<id.row()<<std::endl;
    handle->GimbalPresetLocationTigger(id.row());
    return ;
}

void MainWindow::showPresetLocation(){
    std::cout<<"show preset location:"<<std::endl;
    if (handle == nullptr){
        std::cout<< "camera ctl handle is null"<<std::endl;
    }
    std::vector<GimbalPresetLocation> locations = handle->GimbalPresetLocationGet();

    for (unsigned long long i  = 0; i < locations.size(); i++){
        std::cout<<"id:" << locations[i].id <<"roll:" << locations[i].roll <<" pitch:"<<locations[i].pitch <<" yaw:"<<locations[i].yaw<<std::endl;
        std::string presetLocationItem = "ratio:" + std::to_string(locations[i].zoom) + "     " +
                                         "roll:" + std::to_string( locations[i].roll) + "     " +
                                         "pitch:" + std::to_string(locations[i].pitch) + "     " +
                                         "yaw:" + std::to_string(locations[i].yaw) + "     ";

        std::cout<<"presetLocationgItem:" << presetLocationItem<<std::endl;
        QString presetLocationStr = QString::fromStdString(presetLocationItem);

        QStringList presetLocationList;
        presetLocationList.append(presetLocationStr);

        QStandardItem *item = new QStandardItem(presetLocationStr);
        ItemModel->appendRow(item);
        delete item;

    }

    ui->presetLocationValueList->setModel(ItemModel);
    ui->presetLocationValueList->setFixedWidth(311);

    connect(ui->presetLocationValueList, SIGNAL(clicked(QModelIndex)), this, SLOT(triggerPresetLocation(QModelIndex)));
    //ui->presetLocationValueList->clicked.connect(triggerPresetLocation);
}

void MainWindow::on_gimbalResetButton_clicked()
{
    handle->GimbalLocationReset();
}

void MainWindow::on_zoomButton_clicked()
{
    handle->CameraZoomTigger();
}

void MainWindow::on_gimbalUpButton_clicked()
{
    handle->CameraDirectionSet(0, -3);
}

void MainWindow::on_gimbalLeftButton_clicked()
{
    handle->CameraDirectionSet(5, 0);
}

void MainWindow::on_gimbalDownButton_clicked()
{
    handle->CameraDirectionSet(0, 3);
}

void MainWindow::on_gimbalRightButton_clicked()
{
    handle->CameraDirectionSet(-5, 0);
}
/*
void MainWindow::on_zoomAddButton_clicked()
{
    handle->CameraZoomSet(500);
}

void MainWindow::on_zoomReduceButton_clicked()
{
    handle->CameraZoomSet(-500);
}
*/
void MainWindow::on_autoTrackingButton_clicked()
{
    bool trackingStatus = handle->AITrackingGet();
    if (!trackingStatus){
        ui->autoTrackingButton->setIcon(QIcon(":/image/icon/on2.png"));
        ui->autoTrackingButton->setText("打开");
        ui->pushButton->setEnabled(true);
        std::cout<<"auto tracking is  on "<<std::endl;
    } else {
        ui->autoTrackingButton->setIcon(QIcon(":/image/icon/off2.png"));
        ui->autoTrackingButton->setText("关闭");
        ui->pushButton->setEnabled(false);

        std::cout<<"auto tracking is  off "<<std::endl;

    }
    handle->AITrackingSet(!trackingStatus);
}

void MainWindow::on_handPoseButton_clicked()
{
    bool handPoseStatus = !handle->AIHandposeGet();
    if (handPoseStatus){
        ui->handPoseButton->setIcon(QIcon(":/image/icon/on2.png"));
        ui->handPoseButton->setText("打开");
        std::cout<<"hand pose is  on "<<std::endl;

    } else {
        ui->handPoseButton->setIcon(QIcon(":/image/icon/off2.png"));
        ui->handPoseButton->setText("关闭");
        std::cout<<"hande pose is  off "<<std::endl;

    }
    handle->AIHandposeSet(handPoseStatus);
}

void MainWindow::on_autoZoomButton_clicked()
{
    bool currentZoomStatus = handle->CameraAutoZoomGet();
    if (!currentZoomStatus){
        //auto zoom status disable  handzoom
        ui->zoomAddButton->setEnabled(false);
        ui->zoomReduceButton->setEnabled(false);
        ui->zoomButton->setEnabled(false);
        ui->autoZoomButton->setText("打开");
        ui->autoZoomButton->setIcon(QIcon(":/image/icon/on2.png"));

        std::cout<<"auto zomm is  on "<<std::endl;

    } else {
        //enable hand zoom
        ui->zoomAddButton->setEnabled(true);
        ui->zoomReduceButton->setEnabled(true);
        ui->zoomButton->setEnabled(true);
        std::cout<<"auto zoom is  off "<<std::endl;
        ui->autoZoomButton->setText("关闭");
        ui->autoZoomButton->setIcon(QIcon(":/image/icon/off2.png"));
    }
    handle->CameraAutoZoomSet(!currentZoomStatus);
}

void MainWindow::on_uvcButton_clicked(){


}

void MainWindow::on_objectDefaultViewButton_clicked(){
    bool currentDefaultView = handle->AIDefaultViewGet();
    if (!currentDefaultView){
        //default view landscape
        ui->objectDefaultViewButton->setText("横屏");
        std::cout<<"default view  is  landscape "<<std::endl;

    } else {
        std::cout<<"default view is not landscape "<<std::endl;
        ui->objectDefaultViewButton->setText("竖屏");
    }
    handle->AIDefaultViewSet(!currentDefaultView);
}

void MainWindow::on_witheBalanceComboBox_currentIndexChanged(const QString &arg1){
    std::cout<< "current white balance 0 : "<< ui->witheBalanceComboBox->currentText().toStdString()<<std::endl;
    QString currentWhiteBalance =  Util::GBK2UTF8(ui->witheBalanceComboBox->currentText());
    std::cout<< "current white balance : "<< currentWhiteBalance.toStdString()<<std::endl;
}

void MainWindow::on_exposeComboBox_currentIndexChanged(const QString &arg1){
    std::cout<< "current expose  0 : "<< ui->exposeComboBox->currentText().toStdString()<<std::endl;
    QString currentExpose = Util::GBK2UTF8(ui->exposeComboBox->currentText());
    std::cout<< "current expose  : "<<currentExpose.toStdString()<<std::endl;
}

void MainWindow::on_isoComboBox_currentIndexChanged(const QString &arg1){
    std::cout<< "current iso  0 : "<< ui->isoComboBox->currentText().toStdString()<<std::endl;
    QString currentISO =  Util::GBK2UTF8(ui->isoComboBox->currentText());
    std::cout<< "current iso  : "<<  currentISO.toStdString()<<std::endl;
}


void MainWindow::on_pushButton_clicked(){
    handle->TargetSelect();
    std::cout<<"target select"<<std::endl;
}

void MainWindow::on_gimbalUpLeftButton_clicked(){
     handle->CameraDirectionSet(5, -3);
}

void MainWindow::on_gimbalUpRightButton_clicked(){
     handle->CameraDirectionSet(-5, -3);
}

void MainWindow::on_gimbalDownLeftButton_clicked(){
    handle->CameraDirectionSet(5, 3);
}

void MainWindow::on_gimbalDownRightButton_clicked(){
    handle->CameraDirectionSet(-5, 3);
}

void MainWindow::on_zoomSlider_sliderReleased(){
    //1000-1x   1500-1.5x   2000-2x    5000-5
    handle->CameraZoomSet(currentZoomValue*1000);

    std::cout<<"zoom slider released, set zoom "<< currentZoomValue<<std::endl;
}

void MainWindow::on_zoomSlider_sliderPressed(){
    handle->CameraZoomSet(currentZoomValue*1000);

    std::cout<<"zoom slider pressed"<<std::endl;
}

void MainWindow::on_zoomSlider_sliderMoved(int position){
    currentZoomValue = 0.5*position+1;

    std::string currentZoom = std::to_string(currentZoomValue)+"x";
    ui->zoomLabel_2->setText(QString::fromStdString(currentZoom));
    std::cout<<"zoom slider moved position "<<position<<std::endl;

}
