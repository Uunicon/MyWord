#include "myword.h"
#include <QtWidgets>
#include "mychild.h"

const QString rsrcPath = ":/images";
MyWord::MyWord(QWidget *parent)
    : QMainWindow(parent)
{
    mdiArea = new QMdiArea;
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mdiArea);
    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),this, SLOT(updateMenus()));
    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget*)),this, SLOT(setActiveSubWindow(QWidget*)));

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    updateMenus();

    move(200,150);
    resize(800,500);

    setWindowTitle(tr("Myself Word"));
    setUnifiedTitleAndToolBarOnMac(true);
}

MyWord::~MyWord()
{
    
}

void MyWord::closeEvent(QCloseEvent *event)
{
    mdiArea->closeAllSubWindows();
    if (mdiArea->currentSubWindow()) {
        event->ignore();
    } else {
        event->accept();
    }
}

void MyWord::updateMenus()
{
    //至少有一个子文档打开着的情况
    bool hasMyChild = (activeMyChild()!=0);

    saveAct->setEnabled(hasMyChild);
    saveAsAct->setEnabled(hasMyChild);
    printAct->setEnabled(hasMyChild);
    printPreviewAct->setEnabled(hasMyChild);

    pasteAct->setEnabled(hasMyChild);

    closeAct->setEnabled(hasMyChild);
    closeAllAct->setEnabled(hasMyChild);
    tileAct->setEnabled(hasMyChild);
    cascadeAct->setEnabled(hasMyChild);
    nextAct->setEnabled(hasMyChild);
    previousAct->setEnabled(hasMyChild);
    separatorAct->setVisible(hasMyChild);
    //文档打开着并且其中有内容被选中的情况
    bool hasSelection = (activeMyChild() && activeMyChild()->textCursor().hasSelection());

    cutAct->setEnabled(hasSelection);
    copyAct->setEnabled(hasSelection);

    boldAct->setEnabled(hasSelection);
    italicAct->setEnabled(hasSelection);
    underlineAct->setEnabled(hasSelection);
    leftAlignAct->setEnabled(hasSelection);
    centerAct->setEnabled(hasSelection);
    rightAlignAct->setEnabled(hasSelection);
    justifyAct->setEnabled(hasSelection);
    colorAct->setEnabled(hasSelection);
}

void MyWord::updateWindowMenu()
{
    windowMenu->clear();
    windowMenu->addAction(closeAct);
    windowMenu->addAction(closeAllAct);
    windowMenu->addSeparator();
    windowMenu->addAction(tileAct);
    windowMenu->addAction(cascadeAct);
    windowMenu->addSeparator();
    windowMenu->addAction(nextAct);
    windowMenu->addAction(previousAct);
    windowMenu->addAction(separatorAct);

    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    separatorAct->setVisible(!windows.isEmpty());
    //显示当前打开着的文档子窗口项
    for (int i = 0; i < windows.size(); ++i) {
        MyChild *child = qobject_cast<MyChild *>(windows.at(i)->widget());

        QString text;
        if (i < 9) {
            text = tr("&%1 %2").arg(i + 1).arg(child->userFriendlyCurrentFile());
        } else {
            text = tr("%1 %2").arg(i + 1).arg(child->userFriendlyCurrentFile());
        }
        QAction *action  = windowMenu->addAction(text);
        action->setCheckable(true);
        action ->setChecked(child == activeMyChild());
        connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
        windowMapper->setMapping(action, windows.at(i));
    }
    enabledText();          //使得字体设置菜单可用
}

MyChild *MyWord::createMyChild()
{
    MyChild *child = new MyChild;
    mdiArea->addSubWindow(child);

    connect(child, SIGNAL(copyAvailable(bool)),cutAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(copyAvailable(bool)),copyAct, SLOT(setEnabled(bool)));

    return child;
}

void MyWord::createActions()
{
    /*【文件】菜单动作集*/
    newAct = new QAction(QIcon(rsrcPath + "/filenew.png"), tr("New(&N)"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setToolTip("New");                   //设置工具栏按钮的提示文本
    newAct->setStatusTip(tr("Create a new document"));     //设置状态栏提示文本
    connect(newAct, SIGNAL(triggered()), this, SLOT(fileNew()));

    openAct = new QAction(QIcon(rsrcPath + "/fileopen.png"), tr("Open(&O)..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setToolTip("Open");
    openAct->setStatusTip(tr("Open an existig document"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(fileOpen()));

    saveAct = new QAction(QIcon(rsrcPath + "/filesave.png"), tr("Save(&S)"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setToolTip("Save");
    saveAct->setStatusTip(tr("Save this file"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(fileSave()));

    saveAsAct = new QAction(tr("Save as(&A)..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document with a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(fileSaveAs()));

    printAct = new QAction(QIcon(rsrcPath + "/fileprint.png"), tr("Print(&P)..."), this);
    printAct->setShortcuts(QKeySequence::Print);
    printAct->setToolTip("Print");
    printAct->setStatusTip(tr("Print the file"));
    connect(printAct, SIGNAL(triggered()), this, SLOT(filePrint()));

    printPreviewAct = new QAction(tr("Print Preview..."), this);
    printPreviewAct->setStatusTip(tr("Preview print results"));
    connect(printPreviewAct, SIGNAL(triggered()), this, SLOT(filePrintPreview()));

    exitAct = new QAction(tr("Eixt(&X)"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Eixt this program"));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));           //不用自己实现

    /*【编辑】菜单动作集*/
    undoAct = new QAction(QIcon(rsrcPath + "/editundo.png"),tr("Undo(&U)"), this);
    undoAct->setShortcut(QKeySequence::Undo);
    undoAct->setToolTip("Undo");
    undoAct->setStatusTip(tr("Undo the current operation"));
    connect(undoAct, SIGNAL(triggered()), this, SLOT(undo()));      //不用子窗口类去实现

    redoAct = new QAction(QIcon(rsrcPath + "/editredo.png"),tr("Redo(&R)"), this);
    redoAct->setShortcut(QKeySequence::Redo);
    redoAct->setToolTip("Redo ");
    redoAct->setStatusTip(tr("Redo the previous operation"));
    connect(redoAct, SIGNAL(triggered()), this, SLOT(redo()));      //不用子窗口类去实现

    cutAct = new QAction(QIcon(rsrcPath + "/editcut.png"),tr("Cut(&T)"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setToolTip("Cut");
    cutAct->setStatusTip(tr("Cut the selection from the document and drop it into the clipboard"));
    connect(cutAct, SIGNAL(triggered()), this, SLOT(cut()));        //不用子窗口类去实现

    copyAct = new QAction(QIcon(rsrcPath + "/editcopy.png"),tr("Copy(&C)"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setToolTip("Copy");
    copyAct->setStatusTip(tr("Copy the selection from the document and drop it into the clipboard"));
    connect(copyAct, SIGNAL(triggered()), this, SLOT(copy()));      //不用子窗口类去实现

    pasteAct = new QAction(QIcon(rsrcPath + "/editpaste.png"),tr("Paste(&P)"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setToolTip("Paste");
    pasteAct->setStatusTip(tr("Paste the contents of the clipboard into the document"));
    connect(pasteAct, SIGNAL(triggered()), this, SLOT(paste()));    //不用子窗口类去实现

    CountAct =new QAction(tr("Count"),this);
    CountAct->setStatusTip(tr("Count the number of words"));
    connect(CountAct,SIGNAL(triggered()),this,SLOT(count()));

    FindAct =new QAction(tr("Find"),this);
    FindAct->setStatusTip(tr("Find string in the file"));
    connect(FindAct,SIGNAL(triggered()),this,SLOT(find()));

    /*【格式】菜单动作集*/
    boldAct = new QAction(QIcon(rsrcPath + "/textbold.png"),tr("Bold(&B)"), this);
    boldAct->setCheckable(true);
    boldAct->setShortcut(Qt::CTRL + Qt::Key_B);
    boldAct->setToolTip("Bold");
    boldAct->setStatusTip(tr("Bold the selected text"));
    QFont bold;
    bold.setBold(true);
    boldAct->setFont(bold);
    connect(boldAct, SIGNAL(triggered()), this, SLOT(textBold()));

    italicAct = new QAction(QIcon(rsrcPath + "/textitalic.png"),tr("Italic(&I)"), this);
    italicAct->setCheckable(true);
    italicAct->setShortcut(Qt::CTRL + Qt::Key_I);
    italicAct->setToolTip("Italic");
    italicAct->setStatusTip(tr("Display the selected text in italics"));
    QFont italic;
    italic.setItalic(true);
    italicAct->setFont(italic);
    connect(italicAct, SIGNAL(triggered()), this, SLOT(textItalic()));

    underlineAct = new QAction(QIcon(rsrcPath + "/textunder.png"),tr("Underlin(&U)"), this);
    underlineAct->setCheckable(true);
    underlineAct->setShortcut(Qt::CTRL + Qt::Key_U);
    underlineAct->setToolTip("Underline");
    underlineAct->setStatusTip(tr("Underline the selected text"));
    QFont underline;
    underline.setUnderline(true);
    underlineAct->setFont(underline);
    connect(underlineAct, SIGNAL(triggered()), this, SLOT(textUnderline()));

    //【格式】→【段落】子菜单下的各项为同一个菜单项组，只能选中其中一项
    QActionGroup *grp = new QActionGroup(this);
    connect(grp, SIGNAL(triggered(QAction*)), this, SLOT(textAlign(QAction*)));

    if (QApplication::isLeftToRight()) {
        leftAlignAct = new QAction(QIcon(rsrcPath + "/textleft.png"),tr("leftAlignAct(&L)"), grp);
        centerAct = new QAction(QIcon(rsrcPath + "/textcenter.png"),tr("centerAct(&E)"), grp);
        rightAlignAct = new QAction(QIcon(rsrcPath + "/textright.png"),tr("rightAlignAct(&R)"), grp);
    } else {
        rightAlignAct = new QAction(QIcon(rsrcPath + "/textright.png"),tr("rightAlignAct(&R)"), grp);
        centerAct = new QAction(QIcon(rsrcPath + "/textcenter.png"),tr("centerAct(&E)"), grp);
        leftAlignAct = new QAction(QIcon(rsrcPath + "/textleft.png"),tr("leftAlignAct(&L)"), grp);
    }
    justifyAct = new QAction(QIcon(rsrcPath + "/textjustify.png"),tr("justify(&J)"), grp);

    leftAlignAct->setShortcut(Qt::CTRL + Qt::Key_L);
    leftAlignAct->setCheckable(true);
    leftAlignAct->setToolTip("leftAlign");
    leftAlignAct->setStatusTip(tr("leftAlign the text"));

    centerAct->setShortcut(Qt::CTRL + Qt::Key_E);
    centerAct->setCheckable(true);
    centerAct->setToolTip("center");
    centerAct->setStatusTip(tr("center the text"));

    rightAlignAct->setShortcut(Qt::CTRL + Qt::Key_R);
    rightAlignAct->setCheckable(true);
    rightAlignAct->setToolTip("rightAlign");
    rightAlignAct->setStatusTip(tr("rightAlign the text"));

    justifyAct->setShortcut(Qt::CTRL + Qt::Key_J);
    justifyAct->setCheckable(true);
    justifyAct->setToolTip("justify");
    justifyAct->setStatusTip(tr("justify the text"));

    QPixmap pix(16, 16);
    pix.fill(Qt::red);
    colorAct = new QAction(pix, tr("Color(&C)..."), this);
    colorAct->setToolTip("Color");
    colorAct->setStatusTip(tr("Set Color of the text"));
    connect(colorAct, SIGNAL(triggered()), this, SLOT(textColor()));

    /*【窗口】菜单动作集*/
    closeAct = new QAction(tr("Close(&O)"), this);
    closeAct->setStatusTip(tr("close the active window"));
    connect(closeAct, SIGNAL(triggered()),mdiArea, SLOT(closeActiveSubWindow()));           //不用自己实现

    closeAllAct = new QAction(tr("CloseAll(&A)"), this);
    closeAllAct->setStatusTip(tr("close the all windows"));
    connect(closeAllAct, SIGNAL(triggered()),mdiArea, SLOT(closeAllSubWindows()));          //不用自己实现

    tileAct = new QAction(tr("Tile(&T)"), this);
    tileAct->setStatusTip(tr("tile the windows"));
    connect(tileAct, SIGNAL(triggered()), mdiArea, SLOT(tileSubWindows()));                 //不用自己实现

    cascadeAct = new QAction(tr("Cascade(&C)"), this);
    cascadeAct->setStatusTip(tr("cascade the windows"));
    connect(cascadeAct, SIGNAL(triggered()), mdiArea, SLOT(cascadeSubWindows()));           //不用自己实现

    nextAct = new QAction(tr("Next(&X)"), this);
    nextAct->setShortcuts(QKeySequence::NextChild);
    nextAct->setStatusTip(tr("Next window"));
    connect(nextAct, SIGNAL(triggered()),mdiArea, SLOT(activateNextSubWindow()));           //不用自己实现

    previousAct = new QAction(tr("Previous(&V)"), this);
    previousAct->setShortcuts(QKeySequence::PreviousChild);
    previousAct->setStatusTip(tr("Previous window"));
    connect(previousAct, SIGNAL(triggered()),mdiArea, SLOT(activatePreviousSubWindow()));   //不用自己实现

    separatorAct = new QAction(this);
    separatorAct->setSeparator(true);

    /*【帮助】菜单动作集*/
    aboutAct = new QAction(tr("About(&A)"), this);
    aboutAct->setStatusTip(tr("About Myself Word"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About Qt(&Q)"), this);
    aboutQtAct->setStatusTip(tr("ABout Qt library"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));                        //不用自己实现
}

void MyWord::createMenus()
{
    //【文件】主菜单
    fileMenu = menuBar()->addMenu(tr("File(&F)"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addSeparator();   //分隔线
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();   //分隔线
    fileMenu->addAction(printAct);
    fileMenu->addAction(printPreviewAct);
    fileMenu->addSeparator();   //分隔线
    fileMenu->addAction(exitAct);

    //【编辑】主菜单
    editMenu = menuBar()->addMenu(tr("Edit(&E)"));
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
    editMenu->addSeparator();   //分隔线
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);
    editMenu->addSeparator();   //分隔线
    editMenu->addAction(CountAct);
    editMenu->addAction(FindAct);

    //【格式】主菜单
    formatMenu = menuBar()->addMenu(tr("Format(&O)"));
    fontMenu = formatMenu->addMenu(tr("Font(&D)"));   //【字体】子菜单
    fontMenu->addAction(boldAct);
    fontMenu->addAction(italicAct);
    fontMenu->addAction(underlineAct);
    alignMenu = formatMenu->addMenu(tr("Align"));      //【段落】子菜单
    alignMenu->addAction(leftAlignAct);
    alignMenu->addAction(centerAct);
    alignMenu->addAction(rightAlignAct);
    alignMenu->addAction(justifyAct);

    formatMenu->addAction(colorAct);

    //【窗口】主菜单
    windowMenu = menuBar()->addMenu(tr("Window(&W)"));
    updateWindowMenu();
    connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));
    menuBar()->addSeparator();

    //【帮助】主菜单
    helpMenu = menuBar()->addMenu(tr("Help(&H)"));
    helpMenu->addAction(aboutAct);
    helpMenu->addSeparator();   //分隔线
    helpMenu->addAction(aboutQtAct);
}

void MyWord::createToolBars()
{
    //"文件"工具栏
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);
    fileToolBar->addSeparator();        //分隔条
    fileToolBar->addAction(printAct);

    //"编辑"工具栏
    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(undoAct);
    editToolBar->addAction(redoAct);
    editToolBar->addSeparator();        //分隔条
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);

    //"格式"工具栏
    formatToolBar = addToolBar(tr("Format"));
    formatToolBar->addAction(boldAct);
    formatToolBar->addAction(italicAct);
    formatToolBar->addAction(underlineAct);
    formatToolBar->addSeparator();      //分隔条
    formatToolBar->addAction(leftAlignAct);
    formatToolBar->addAction(centerAct);
    formatToolBar->addAction(rightAlignAct);
    formatToolBar->addAction(justifyAct);
    formatToolBar->addSeparator();      //分隔条
    formatToolBar->addAction(colorAct);

    //组合工具栏
    addToolBarBreak(Qt::TopToolBarArea);    //使这个工具条在界面上另起一行显示
    comboToolBar = addToolBar(tr("Symbol and Number"));
    comboStyle = new QComboBox();
    comboToolBar->addWidget(comboStyle);
    comboStyle->addItem("Standard");
    comboStyle->addItem("Item Symbol (.)");
    comboStyle->addItem("Item Symbol (。)");
    comboStyle->addItem("Item Symbol (■)");
    comboStyle->addItem("Number (1.2.3.)");
    comboStyle->addItem("Number ( a.b.c.)");
    comboStyle->addItem("Number ( A.B.C.)");
    comboStyle->addItem("Number (ⅰ.ⅱ.ⅲ.)");
    comboStyle->addItem("Number (Ⅰ.Ⅱ.Ⅲ.)");
    comboStyle->setStatusTip("Add symbol or number");
    connect(comboStyle, SIGNAL(activated(int)), this, SLOT(textStyle(int)));

    comboFont = new QFontComboBox();
    comboToolBar->addWidget(comboFont);
    comboFont->setStatusTip("Change Font");
    connect(comboFont, SIGNAL(activated(QString)), this, SLOT(textFamily(QString)));

    comboSize = new QComboBox();
    comboToolBar->addWidget(comboSize);
    comboSize->setEditable(true);
    comboSize->setStatusTip("Change font size");

    QFontDatabase db;
    foreach(int size, db.standardSizes())
        comboSize->addItem(QString::number(size));

    connect(comboSize, SIGNAL(activated(QString)), this, SLOT(textSize(QString)));
    comboSize->setCurrentIndex(comboSize->findText(QString::number(QApplication::font().pointSize())));
}

void MyWord::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

MyChild *MyWord::activeMyChild()
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
        return qobject_cast<MyChild *>(activeSubWindow->widget());
    return 0;
}

//打开文件用
QMdiSubWindow *MyWord::findMyChild(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
        MyChild *myChild = qobject_cast<MyChild *>(window->widget());
        if (myChild->currentFile() == canonicalFilePath)
            return window;
    }
    return 0;
}
void MyWord::setActiveSubWindow(QWidget *window)
{
    if (!window)
        return;
    mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

void MyWord::fileNew()
{
    MyChild *child = createMyChild();
    child->newFile();
    child->show();
    enabledText();              //使得字体设置菜单可用
}

void MyWord::fileOpen()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("打开"),QString(), tr("HTML 文档 (*.htm *.html);;所有文件 (*.*)"));
    if (!fileName.isEmpty()) {
        QMdiSubWindow *existing = findMyChild(fileName);
        if (existing) {
            mdiArea->setActiveSubWindow(existing);
            return;
        }
        MyChild *child = createMyChild();
        if (child->loadFile(fileName)) {
            statusBar()->showMessage(tr("File Loaded"), 2000);
            child->show();
            enabledText();      //使得字体设置菜单可用
        } else {
            child->close();
        }
    }    
}

void MyWord::fileSave()
{
    if (activeMyChild() && activeMyChild()->save())
        statusBar()->showMessage(tr("Saved successfully"), 2000);
}

void MyWord::fileSaveAs()
{
    if (activeMyChild() && activeMyChild()->saveAs())
        statusBar()->showMessage(tr("Saved successfully"), 2000);
}

void MyWord::undo()
{
    if(activeMyChild())
        activeMyChild()->undo();
}

void MyWord::redo()
{
    if(activeMyChild())
        activeMyChild()->redo();
}

void MyWord::cut()
{
    if (activeMyChild())
        activeMyChild()->cut();
}

void MyWord::copy()
{
    if (activeMyChild())
        activeMyChild()->copy();
}

void MyWord::paste()
{
    if (activeMyChild())
        activeMyChild()->paste();
}

void MyWord::enabledText()
{
    boldAct->setEnabled(true);
    italicAct->setEnabled(true);
    underlineAct->setEnabled(true);
    leftAlignAct->setEnabled(true);
    centerAct->setEnabled(true);
    rightAlignAct->setEnabled(true);
    justifyAct->setEnabled(true);
    colorAct->setEnabled(true);
}

void MyWord::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(boldAct->isChecked() ? QFont::Bold : QFont::Normal);
    if(activeMyChild())
        activeMyChild()->mergeFormatOnWordOrSelection(fmt);
}

void MyWord::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(italicAct->isChecked());
    if(activeMyChild())
        activeMyChild()->mergeFormatOnWordOrSelection(fmt);
}

void MyWord::textUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(underlineAct->isChecked());
    if(activeMyChild())
        activeMyChild()->mergeFormatOnWordOrSelection(fmt);
}

void MyWord::textAlign(QAction *a)
{
    if(activeMyChild())
    {
        if (a == leftAlignAct)
            activeMyChild()->setAlign(1);
        else if (a == centerAct)
            activeMyChild()->setAlign(2);
        else if (a == rightAlignAct)
            activeMyChild()->setAlign(3);
        else if (a == justifyAct)
            activeMyChild()->setAlign(4);
    }
}

void MyWord::textStyle(int styleIndex)
{
    if(activeMyChild())
    {
        activeMyChild()->setStyle(styleIndex);
    }
}

void MyWord::textFamily(const QString &f)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    if(activeMyChild())
        activeMyChild()->mergeFormatOnWordOrSelection(fmt);
}

void MyWord::textSize(const QString &p)
{
    qreal pointSize = p.toFloat();
    if (p.toFloat() > 0) {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        if(activeMyChild())
            activeMyChild()->mergeFormatOnWordOrSelection(fmt);
    }
}

void MyWord::textColor()
{
    if(activeMyChild())
    {
        QColor col = QColorDialog::getColor(activeMyChild()->textColor(), this);
        if (!col.isValid())
            return;
        QTextCharFormat fmt;
        fmt.setForeground(col);
        activeMyChild()->mergeFormatOnWordOrSelection(fmt);
        colorChanged(col);
    }
}

void MyWord::fontChanged(const QFont &f)
{
    comboFont->setCurrentIndex(comboFont->findText(QFontInfo(f).family()));
    comboSize->setCurrentIndex(comboSize->findText(QString::number(f.pointSize())));
    boldAct->setChecked(f.bold());
    italicAct->setChecked(f.italic());
    underlineAct->setChecked(f.underline());
}

void MyWord::colorChanged(const QColor &c)
{
    QPixmap pix(16, 16);
    pix.fill(c);
    colorAct->setIcon(pix);
}

void MyWord::alignmentChanged(Qt::Alignment a)
{
    if (a & Qt::AlignLeft)
        leftAlignAct->setChecked(true);
    else if (a & Qt::AlignHCenter)
        centerAct->setChecked(true);
    else if (a & Qt::AlignRight)
        rightAlignAct->setChecked(true);
    else if (a & Qt::AlignJustify)
        justifyAct->setChecked(true);
}

void MyWord::filePrint()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (activeMyChild()->textCursor().hasSelection())
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    dlg->setWindowTitle(tr("Print the file"));
    if (dlg->exec() == QDialog::Accepted)
        activeMyChild()->print(&printer);
    delete dlg;
}

void MyWord::filePrintPreview()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(printPreview(QPrinter*)));
    preview.exec();
}

void MyWord::printPreview(QPrinter *printer)
{
    activeMyChild()->print(printer);
}

void MyWord::about()
{
    QMessageBox::about(this, tr("About"), tr("这是一个基于 Qt 实现的文字处理软件,具备类似 微软Office Word 的功能."));
}

void MyWord::count()
{
    int L = 0;//大写字母个数
    int l=0; //小写字母
    int N = 0;//数字个数
    int O = 0;//其他字符
    int sp = 0;//空格字数
    int C = 0;//汉字字数
    QString str;
    if(activeMyChild())
    {
        str=activeMyChild()->toPlainText();
        int len = str.length();
        for(int i = 0; i < len; ++i)
        {
            QChar t = str.at(i);
            ushort uni = t.unicode();
            if(uni >= 0x4E00 && uni <= 0x9FA5)
                C++; //中文
            else if (uni >= 0x0030 && uni <= 0x0039)
                N++; //数字
            else if (uni >= 97 && uni <= 122)
                l++; //小写字母
            else if (uni >= 65 && uni <= 90)
                L++;  //大写字母
            else if (uni ==32)
                sp++;  //空格
            else
                O++;  //其他字符
         }
        //message对话框
        QString mesg=tr(" 大写字母:  %1\n 小写字母:  %2\n 数字:     %3\n 汉字:     %4 \n 空格:     %5 \n 其他字符:  %6 \n 总字符数:  %7 \n ").arg(L).arg(l).arg(N).arg(C).arg(sp).arg(O).arg(len);    // m = "12:60:60:;
        QMessageBox::about(NULL, "Count", mesg);
    }
}

void MyWord::find()
{
    QString strfile;
    if(activeMyChild())
    {
        strfile=activeMyChild()->toPlainText();
        //将文件从String转换为char*
        std::string str = strfile.toStdString();
        const char* filetext = str.c_str();

        bool ok;
        QString text=QInputDialog::getText(this,tr("Find string"),tr("Please input a string"),QLineEdit::Normal,nullptr,&ok);
        //将输入从String转换为char*
        std::string str2 = text.toStdString();
        const char* inputtext = str2.c_str();

        if(ok && !text.isEmpty())
        {
            int number = Num0fstr(filetext,inputtext);
            //message对话框
            QString mesg2=tr(" Number of string:  %1\n ").arg(number);
            QMessageBox::about(NULL, "Find", mesg2);
        }
    }

}

int MyWord::Num0fstr(const char *Mstr, const char *substr)
{
    int number = 0;
        const char *p;//字符串辅助指针
        const char *q;//字符串辅助指针
        while(*Mstr != '\0')
        {
            p = Mstr;
            q = substr;

            while((*p == *q)&&(*p!='\0')&&(*q!='\0'))
            { //相等且未到字符串结尾
                p++;
                q++;
            }
            if(*q=='\0')
            {
                number++; //计数加一
            }
            Mstr++;
        }
        return number;
}
