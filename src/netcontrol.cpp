/*
  Copyright © 2021 Hasan Yavuz Özderya

  This file is part of serialplot.

  serialplot is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  serialplot is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with serialplot.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "netcontrol.h"
#include "ui_netcontrol.h"

#include <QKeySequence>
#include <QLabel>
#include <QLineEdit>
#include <QMap>
#include <QtDebug>

#include "setting_defines.h"
#include "utils.h"

NetControl::NetControl(QTcpSocket *client, QWidget *parent) : QWidget(parent),
                                                              ui(new Ui::NetControl),
                                                              netToolBar("Net Toolbar"),
                                                              openAction("Open", this)
{
    ui->setupUi(this);

    tcpClient = client;

    // setup actions
    openAction.setCheckable(true);
    // openAction.setShortcut(QKeySequence("F12"));
    openAction.setToolTip("Net");
    QObject::connect(&openAction, &QAction::triggered,
                     this, &NetControl::openActionTriggered);

    netToolBar.addAction(&openAction);

    ui->pbNetOpen->setDefaultAction(&openAction);

    // ui->leNetIP->setInputMask("000.000.000.000; ");  
    ui->leNetPort->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));   

    //connect(tcpclient, &QTcpSocket::readyRead, this, &MainWindow::tcpclientreaddata);
}

NetControl::~NetControl()
{
    delete ui;
}

void NetControl::openActionTriggered(bool checked)
{
    QString ip = ui->leNetIP->text();
    int port = ui->leNetPort->text().toInt();


    qDebug() << "IP:  " << ip;
    // qDebug() << "PORT:" << port;

    if(tcpClient->state() != QAbstractSocket::ConnectedState)
    {
        tcpClient->connectToHost(ip, port);

        if (!tcpClient->waitForConnected(1000))
        {
            qDebug() << "Connection failed!";

            openAction.setChecked(false);

            return;
        }

        openAction.setChecked(true);
        emit selectReaderDevice(1);

    }else{
        tcpClient->close();
        openAction.setChecked(false);
    }
}

QToolBar *NetControl::toolBar()
{
    return &netToolBar;
}

void NetControl::saveSettings(QSettings* settings)
{
    settings->beginGroup(SettingGroup_Net);
    settings->setValue(SG_Net_IP, ui->leNetIP->text());
    settings->setValue(SG_Net_Port, ui->leNetPort->text());
    settings->endGroup();
}

void NetControl::loadSettings(QSettings* settings)
{

    settings->beginGroup(SettingGroup_Net);

    QString ip = settings->value(SG_Net_IP, QString()).toString();
    if (!ip.isEmpty())
    {
        ui->leNetIP->setText(ip);
    }

    QString port = settings->value(SG_Net_Port, QString()).toString();
    if (!port.isEmpty())
    {
        ui->leNetPort->setText(port);
    }

    settings->endGroup();
}
