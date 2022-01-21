#include "shortcuts_window.h"
#include <QKeyEvent>
#include <QFont>
#include <QFontDatabase>
#include <QStyle>

ShortCutsWindow::ShortCutsWindow(QWidget *parent) : QWidget(parent)
{
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    setWindowTitle(tr("Keyboard and Mouse Settings"));

    box = new QGroupBox(tr("Mouse, key, and shortcuts settings"), this);
    box->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    box->setAlignment(Qt::AlignHCenter);
    grid = new QGridLayout(this);

    shortcutsgrid = new QGridLayout(box);
    shortcutsgrid->setSpacing(10);
    shortcutsgrid->setContentsMargins(20, 20, 20, 20);

    shortcuts_table = new QTableWidget(this);

    QStringList header;
    header << tr("Binding") << tr("Function");
    shortcuts_table->setColumnCount(2);
    shortcuts_table->setHorizontalHeaderLabels(header);
    shortcuts_table->setSelectionMode(QAbstractItemView::NoSelection);
    shortcuts_table->setAlternatingRowColors(true);
    shortcuts_table->setSizeAdjustPolicy(QTableWidget::AdjustToContents);
    shortcuts_table->setColumnWidth(0, 230);
    shortcuts_table->setColumnWidth(1, 625);
    shortcuts_table->setMinimumWidth(720);
    shortcuts_table->setMaximumHeight(620);
    shortcuts_table->setAutoScroll(true);

    font2 = font = shortcuts_table->font();
    font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    font.setStyleHint(QFont::TypeWriter);
    font2.setItalic(true);

    add_binding(tr("Alt  +  ↑"), tr("Viewport: Scene tilt up"));
    add_binding(tr("Alt  +  ↓"), tr("Viewport: Scene tilt down"));
    add_binding(tr("Alt  +  ←"), tr("Viewport: Scene pan left along the Z axis"));
    add_binding(tr("Alt  +  →"), tr("Viewport: Scene pan right along the Z axis"));
    add_binding(tr("Shift  +  ↑"), tr("Viewport: Scene tilt up"));
    add_binding(tr("Shift  +  ↓"), tr("Viewport: Scene tilt down"));
    add_binding(tr("Shift  +  ←"), tr("Viewport: Scene pan left along the Y axis"));
    add_binding(tr("Shift  +  →"), tr("Viewport: Scene pan right along the Y axis"));
    add_binding(tr(" ↑"), tr("Viewport: Move the viewpoint up"));
    add_binding(tr(" ↓"), tr("Viewport: Move the viewpoint down"));
    add_binding(tr(" ←"), tr("Viewport: Move the viewpoint left"));
    add_binding(tr(" →"), tr("Viewport: Move the viewpoint right"));
    add_binding(tr("Page Up"), tr("Viewport: Zoom into the scene"));
    add_binding(tr("Page Down"), tr("Viewport: Zoom out of the scene"));
    add_binding(tr("Escape"), tr("Viewport: Fit the scene in the viewport"));
    add_binding(tr("Escape"), tr("Hide any subwindow"));
    add_binding(tr("Ctrl  +  K"), tr("Show the Keyboard, Mouse bindings, and Shortcuts window"));
    add_binding(tr("Ctrl  +  P"), tr("Show the Preferences window"));
    add_binding(tr("Ctrl  +  S"), tr("Show the Spectrum window"));
    add_binding(tr("Ctrl  +  M"), tr("Toggle the rotate buttons from interactive to animation mode"));
    add_binding(tr("Ctrl  +  N"), tr("Show normal mode vectors"));
    add_binding(tr("Ctrl  +  X"), tr("Viewport: Show the molecule fixed axes with optional transparency."));
    add_binding(tr("Right Mouse Down"), tr("Viewport: Pan around the Y axis and tilt while moving the mouse"));
    add_binding(tr("Alt + Right Mouse Down"), tr("Viewport: Pan around the Z axis and tilt while moving the mouse"));
    add_binding(tr("Left Mouse Down"), tr("Viewport: Translate the scene in the plane of the viewpoint while moving the mouse"));
    add_binding(tr("Left + Right Mouse Down"), tr("Viewport: Zoom in and out while moving the mouse up and down"));
    add_binding(tr("Mouse Wheel"), tr("Viewport: Zoom in and out of the scene."));
    add_binding(tr("Ctrl  +  O"), tr("Open the file dialog"));
    add_binding(tr("Ctrl  +  Q"), tr("Exit the application"));
    add_binding(tr("Ctrl  +  W"), tr("Save a snapshot of a nomal mode in xyz format"));
    add_binding(tr("Ctrl  +  D"), tr("Restore default animation settings"));
    add_binding(tr("Ctrl  +  A"), tr("Restore default atom and bond scaling"));
    add_binding(tr("Ctrl  +  R"), tr("Restore default camera settings"));
    add_binding(tr("Ctrl  +  Y"), tr("Enable automatic play whe selecting a normal mode"));
    add_binding(tr(" -"), tr("Viewport: Switch the rotate buttons to anti-clockwise rotation"));
    add_binding(tr(" ="), tr("Viewport: Switch the rotate buttons to clockwise rotation"));

    shortcuts_table->sortByColumn(0, Qt::SortOrder::AscendingOrder);

    grid->addWidget(box, 0, 0);
    shortcutsgrid->addWidget(shortcuts_table, 0, 0);
    setLayout(grid);
}

void ShortCutsWindow::add_binding(const QString& binding, const QString& desc)
{
    shortcuts_table->insertRow(count);

    QTableWidgetItem *newItem = new QTableWidgetItem;
    newItem->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    newItem->setText(binding);
    newItem->setFont(font);
    shortcuts_table->setItem(count, 0, newItem);

    QTableWidgetItem *newItem2 = new QTableWidgetItem;
    newItem2->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    newItem2->setText(desc);
    newItem2->setFont(font2);
    shortcuts_table->setItem(count, 1, newItem2);

    ++count;
}

void ShortCutsWindow::keyPressEvent(QKeyEvent *event)
{
   if (event->key() == Qt::Key_Escape)
       hide();

   QWidget::keyPressEvent(event);
}
