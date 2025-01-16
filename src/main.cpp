#include <QApplication>
#include <QWidget>
#include <QIcon>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QStyleFactory>
#include <QStyle>
#include <QPalette>
#include <filesystem>
#include <qabstracteventdispatcher.h>
#include <qaction.h>
#include <qevent.h>
#include <qgraphicsanchorlayout.h>
#include <qgridlayout.h>
#include <qimage.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlistwidget.h>
#include <qmainwindow.h>
#include <qmenu.h>
#include <qnamespace.h>
#include <QListWidgetItem>
#include <QKeyEvent>
#include <QMenu>
#include <qthread.h>
#include <string>
#include <QInputDialog>
#include <QMessageBox>
#include <fstream>

static void setTheme();
static void generateFileGraphics(QListWidget* list, std::string path);
static void showContextMenu(const QPoint &pos, QListWidget *listWidget, std::string &basePath);

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QMainWindow mainWindow;
    QWidget *centralWidget = new QWidget(&mainWindow);

    setTheme();
    std::string path = "/home/"; 

    QListWidget *widgetList = new QListWidget();
    widgetList->setViewMode(QListWidget::IconMode);
    widgetList->setIconSize(QSize(100,1000));
    widgetList->setResizeMode(QListWidget::Adjust);

    widgetList->setContextMenuPolicy(Qt::CustomContextMenu);

    QObject::connect(widgetList, &QListWidget::customContextMenuRequested, [widgetList, path](const QPoint &pos) mutable {
        showContextMenu(pos, widgetList, path);
    });


    QLineEdit *lineEdit = new QLineEdit();
    mainWindow.setCentralWidget(centralWidget);
    lineEdit->setPlaceholderText("Enter text here...");
    QPushButton *searchBTN = new QPushButton();
    searchBTN->setText("Search");

    QObject::connect(searchBTN, &QPushButton::clicked, [widgetList, lineEdit, searchBTN, &path]() {
        widgetList->clear();
        path = lineEdit->text().toStdString();
        qDebug() << path << "\n" << "\n";
        if(std::filesystem::exists(path)){
            generateFileGraphics(widgetList ,path);
        }else{
            QMessageBox::critical(widgetList,"Error","Directory doesn't exist");
        }
    });

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(lineEdit); 
    layout->addWidget(searchBTN); 

    centralWidget->setLayout(layout);
    layout->addWidget(widgetList);

    mainWindow.show();
    generateFileGraphics(widgetList ,path);

    return app.exec();
}

void setTheme() {

    QStyle* style = QStyleFactory::create("Fusion");
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(53, 53, 53));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(25, 25, 25));
    palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    palette.setColor(QPalette::ToolTipBase, Qt::black);
    palette.setColor(QPalette::ToolTipText, Qt::white);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(53, 53, 53));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Link, QColor(42, 130, 218));
    palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    palette.setColor(QPalette::HighlightedText, Qt::black);
    QApplication::setStyle(style);
    QApplication::setPalette(palette);
}
void generateFileGraphics(QListWidget* list, std::string path){
    std::string itemName, itemIcon;
    QListWidgetItem *item;
    for (const auto & entry : std::filesystem::directory_iterator(path)){

        itemName = entry.path().filename();
        if(entry.is_directory()){
            itemIcon = "/home/erizo/dev/c++/FileManager/res/directory.png";
        }
        else if(itemName.ends_with("png") || itemName.ends_with("jpg") || itemName.ends_with("jpeg") || itemName.ends_with("webp") || itemName.ends_with("gif")|| itemName.ends_with("webm")){
            itemIcon = entry.path();
        }
        else{
            itemIcon = "/home/erizo/dev/c++/FileManager/res/file.png";
        }
        QIcon icon(QString::fromStdString(itemIcon));
        item = new QListWidgetItem(icon,QString::fromStdString(itemName)); 
        list->setDragEnabled(false);
        list->addItem(item);
        item->setSizeHint(QSize(150,150));
    }
}
void showContextMenu(const QPoint &pos, QListWidget *listWidget, std::string &basePath) {
    QMenu contextMenu;
    QAction *open = contextMenu.addAction("Open");
    QAction *goBack = contextMenu.addAction("Back");
    QAction *createFolder = contextMenu.addAction("Create Folder");
    QAction *createFile = contextMenu.addAction("Create File");
    QAction *renameObject = contextMenu.addAction("Rename Object");
    QAction *removeObject = contextMenu.addAction("Remove Object");

    QAction *selectedAction = contextMenu.exec(listWidget->mapToGlobal(pos));

    if (selectedAction == createFolder) {
        bool ok;
        QString folderName = QInputDialog::getText(listWidget, 
            "Create Folder", 
            "Enter folder name:", 
            QLineEdit::Normal, 
            "", &ok);

        if (ok && !folderName.isEmpty()) {
            std::string fullPath = basePath + "/" + folderName.toStdString();

            try {
                if (std::filesystem::create_directory(fullPath)) {
                    QMessageBox::information(listWidget, "Success", "Folder created successfully.");
                    listWidget->clear();
                    generateFileGraphics(listWidget, basePath);
                } else {
                    QMessageBox::warning(listWidget, "Error", "Failed to create folder.");
                }
            } catch (const std::exception &e) {
                QMessageBox::critical(listWidget, "Error", e.what());
            }
        }
    } else if (selectedAction == createFile) {
        bool ok;
        QString fileName = QInputDialog::getText(listWidget, 
            "Create File", 
            "Enter File name:", 
            QLineEdit::Normal, 
            "", &ok);

        if (ok && !fileName.isEmpty()) {
            std::string fullPath = basePath + fileName.toStdString();

            try {
                std::ofstream file(fullPath);
                if (file.good()) {
                    QMessageBox::information(listWidget, "Success", "File created successfully.");
                    listWidget->clear();
                    generateFileGraphics(listWidget, basePath);
                } else {
                    QMessageBox::warning(listWidget, "Error", "Failed to create folder.");
                }
            } catch (const std::exception &e) {
                QMessageBox::critical(listWidget, "Error", e.what());
            }
        }
    } else if (selectedAction == renameObject) {
        bool ok;
        QString fileName = QInputDialog::getText(listWidget, 
            "Rename File", 
            "Enter New File name:", 
            QLineEdit::Normal, 
            "", &ok);

        if (ok && !fileName.isEmpty()) {
            std::string fullPath = basePath + listWidget->currentItem()->text().toStdString();
            try {
                std::filesystem::rename(fullPath,basePath + fileName.toStdString());
                generateFileGraphics(listWidget, basePath);
            } catch (const std::exception &e) {
                QMessageBox::critical(listWidget, "Error", e.what());
            }
        }
    } else if (selectedAction == removeObject) {
        std::string fullPath = basePath + listWidget->currentItem()->text().toStdString();
        try {
            if(std::filesystem::remove(fullPath)){
                QMessageBox::information(listWidget, "Success", "File removed successfully.");
                listWidget->clear();
                generateFileGraphics(listWidget, basePath);
            } else {
                QMessageBox::warning(listWidget, "Error", "Failed to remove File.");
            }
        } catch (const std::exception &e) {
            QMessageBox::critical(listWidget, "Error", e.what());
        }
    } else if (selectedAction == open) {
        std::string fullPath = basePath + listWidget->currentItem()->text().toStdString();
        try {
            if(std::filesystem::is_directory(fullPath)){
                listWidget->clear();
                generateFileGraphics(listWidget, fullPath);
                qDebug() << basePath << "\n";
                basePath = fullPath + "/";
                qDebug() << fullPath << "\n";
            }else{
                qDebug() << fullPath << "\n";
                std::string str = "xdg-open " + fullPath;
                const char* command = str.c_str();
                system(command);

            }
        } catch (const std::exception &e) {
            QMessageBox::critical(listWidget, "Error", e.what());
        }
    } else if (selectedAction == goBack) {
        std::string fullPath = basePath + listWidget->currentItem()->text().toStdString();
        try {
            listWidget->clear();
            generateFileGraphics(listWidget, fullPath.substr(0,fullPath.rfind('/')));

            qDebug() << fullPath.substr(0,fullPath.find_last_of("/",0)) << "\n";

            basePath = fullPath + "/";
        
        } catch (const std::exception &e) {
            QMessageBox::critical(listWidget, "Error", e.what());
        }
    }
}
