#include "workermanager.h"
#include "ui_workermanager.h"

WorkerManager::WorkerManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WorkerManager)
{
    ui->setupUi(this);
}

WorkerManager::~WorkerManager()
{
    delete ui;
}

void WorkerManager::appendToQueue()
{
    int i = mainQueue.size() + 1;
    mainQueue.append(i);
}
