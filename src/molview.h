#ifndef MOLVIEW_H
#define MOLVIEW_H

#include "qcustom3D/molview3D_atom.h"
#include "qcustom3D/molview3D_bond.h"
#include "qcustom3D/molview3D_axes.h"
#include <string>
#include <QString>
#include <QtCore/QObject>
#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QDial>
#include <Qt3DRender/qpointlight.h>
#include <Qt3DRender/QCamera>
#include <QVector3D>

using Qt3DCustom::AtomMesh;
using Qt3DCustom::BondMesh;
using Qt3DCustom::AxisMesh;

class MolView : public QWidget
{
    Q_OBJECT

public:
    explicit MolView(Qt3DCore::QEntity *rootEntity,
                     Qt3DRender::QCamera *cameraEntity);

    ~MolView() override;

    void openFile(const QString& file, const QString& suffix);
    void play_normal_mode(QTableWidget *, float);
    void set_norm_vectors(QTableWidget *, QCheckBox *, float);
    void stop_normal_mode();
    void resetView(QComboBox *);
    void RotateX(float direction);
    void RotateY(float direction);
    void RotateZ(float direction);
    void RotateX(QPushButton *, QComboBox *, float, bool);
    void RotateY(QPushButton *, QComboBox *, float, bool);
    void RotateZ(QPushButton *, QComboBox *, float, bool);
    void views(QComboBox *);
    void reset();
    void play_xyz_animation(QSpinBox *, QPushButton *);
    void scale_atoms(QDial *);
    void write_xyz(const QString& file);
    void reveal_axes(bool, bool see_through = false);
    void resetViewCentre() const { m_camera->setViewCenter(QVector3D(0, 0, 0));}
    void setConnectivityFromLastFrame(bool last) { connectivity_from_last_frame = last; }
    void setSortAxes(bool sort) { sort_axes = sort; }
    void setAlignAxes(bool align) { align_axes = align; }

public
Q_SLOTS:
    void showFrame(QSpinBox *);
    void sendFileInfo(const QString&);
    void sendFrequencies();
    int  get_frames_count() { return num_frames; }

Q_SIGNALS:
    void file_info_changed(const QString&);
    void frequencies_changed(const QVector<double>&, const QVector<double>&);

private:
    Qt3DCore::QEntity    *m_rootEntity;
    Qt3DRender::QCamera  *m_camera;
    QVector<AtomMesh *> atoms;
    QVector<BondMesh *> bonds;
    QVector<AxisMesh *> axes;
    QVector<AxisMesh *> mode_vectors1;
    QVector<AxisMesh *> mode_vectors2;
    QVector<QVector<QVector3D>> coords;
    QVector<QVector<QVector3D>> normal_modes;
    QVector<double> frequencies;
    QVector<double> intensities;
    QVector<size_t> zvals;
    QVector<int> axes_idx;
    QVector<QVector<bool>> connectivity;
    QVector<QVector<int>> bond_order;
    int num_frames{0};
    int num_atoms{0};
    float largest_distance{0};
    float scale{0};
    double step{0};
    bool export_original_axes{true};
    bool connectivity_from_last_frame{true};
    bool sort_axes{true};
    bool align_axes{true};

    void set_atom_coords(int frame);
    void set_bond_coords(int frame);
    void init_renderFunc(const QString& file,  const QString& suffix);
    void cleanup();

//private slots:
//    void camera_Changed();
};

#endif // MolView_H

