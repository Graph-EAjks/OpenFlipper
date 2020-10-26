#ifndef PLANEPLUGIN_HH
#define PLANEPLUGIN_HH

#include <QObject>

#include <ObjectTypes/Plane/Plane.hh>
#include <ObjectTypes/Plane/QtPlaneSelect.hh>
#include <ObjectTypes/PolyMesh/PolyMesh.hh>
#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>
#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/KeyInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/MouseInterface.hh>
#include <OpenFlipper/BasePlugin/PickingInterface.hh>
#include <OpenFlipper/BasePlugin/ScriptInterface.hh>
#include <OpenFlipper/BasePlugin/ToolbarInterface.hh>
#include <OpenFlipper/common/Types.hh>

class PlanePlugin : public QObject,
                    BaseInterface,
                    MouseInterface,
                    KeyInterface,
                    PickingInterface,
                    LoggingInterface,
                    ToolbarInterface,
                    ScriptInterface,
                    LoadSaveInterface {
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(MouseInterface)
  Q_INTERFACES(KeyInterface)
  Q_INTERFACES(PickingInterface)
  Q_INTERFACES(LoggingInterface)
  Q_INTERFACES(ToolbarInterface)
  Q_INTERFACES(ScriptInterface)
  Q_INTERFACES(LoadSaveInterface)

  Q_PLUGIN_METADATA(IID "org.OpenFlipper.Plugins.Plugin-Plane")

signals:
  // BaseInterface
  void updateView();
  void updatedObject(int, UpdateType);
  void setSlotDescription(QString _slotName, QString _slotDescription,
                          QStringList _parameters, QStringList _descriptions);

  // LoadSaveInterface
  void addEmptyObject(DataType _type, int &_id);
  void copyObject(int _oldId, int &_newId);
  void deleteObject(int _id);

  // ToolBarInterface
  void addToolbar(QToolBar *_toolbar);
  void getToolBar(QString _name, QToolBar *&_toolbar);

  // ToolboxInterface
  void defineViewMode(QString _mode, QStringList _usedWidgets);

  // KeyInterfae
  void registerKey(int _key, Qt::KeyboardModifiers _modifiers,
                   QString _description, bool _multiUse = false);

  // PickingInterface
  void addPickMode(const std::string _mode);

  // ScriptInterface
  void scriptInfo(QString _functionName);

  // LoggingInterface
  void log(Logtype _type, QString _message);
  void log(QString _message);

private slots:

  // BaseInterface
  void initializePlugin();
  void pluginsInitialized();

  // PickingInterface
  void slotPickModeChanged(const std::string &_mode);

  // MouseInterface:
  void slotMouseEvent(QMouseEvent *_event);

  // KeyInterface:
  void slotKeyReleaseEvent(QKeyEvent *_event);

  bool getIntersectionParams(BaseObjectData &_obj, ACG::Vec3d &_center,
                             double &_radius);

public:
  PlanePlugin();
  ~PlanePlugin() {}

  void exit() {}

  QString name() { return (QString("Plane")); }
  QString description() { return (QString("Handle plane objects.")); }

  //===========================================================================
  /** @name Template Functions
   * @{ */
  //===========================================================================

  /// get the points from the intersection between mesh and plane
  template <class MeshT>
  std::vector<ACG::Vec3d>
  getIntersectionPoints(MeshT *_mesh, uint _fh, ACG::Vec3d _planeNormal,
                        ACG::Vec3d _planePoint, bool &_closed);

  /// get an edge of the mesh that is cut by the plane
  template <class MeshT>
  typename MeshT::EdgeHandle getCuttedEdge(MeshT &_mesh,
                                           ACG::Vec3d &_planeNormal,
                                           ACG::Vec3d &_planePoint);

  /** @} */

  //===========================================================================
  /** @name GUI Related
   * @{ */
  //===========================================================================

private slots:

  /// Plane Node Button
  void slotCreatePlaneNode();
  /// Create a plane node when position/normal have been drawn
  void slotCreatePlaneTriggered();

private:
  // Plane Selection Tools
  QtPlaneSelect *createPlane_;

  QAction *planeNodeAction_;

  /// additional information
  // In Picking Mode for plane resize, are we currently dragging or not
  bool dragging_;

  // In Picking Mode for plane resize, which corner did we pick?
  int pickedCorner_;

  // world space coords of the dragging start position (through raycast)
  ACG::Vec3d wMouseDownPosition_{.0};
  ACG::Vec3d wCurrMousePos_{.0};
  ACG::Vec3d viewDirection_{.0};
  // currently dragged plane
  PlaneObject *curPlane_ = nullptr;
  // copy of the plane, to avoid getting lost in floating point error realm
  Plane origPlane_;
  // needed for notifying the renderer to draw the plane correctly
  int lastObjId_ = -1;
  /** @} */

  //===========================================================================
  /** @name Scripting Functions
   * @{ */
  //===========================================================================

public slots:
  QString version() { return QString("1.0"); }

  /** @} */

private:
  /// set scripting slot descriptions
  void setDescriptions();
};

#if defined(INCLUDE_TEMPLATES) && !defined(PLANEPLUGIN_CC)
#define PLANEPLUGIN_TEMPLATES
#include "PlanePluginT_impl.hh"
#endif
//=============================================================================
#endif // PLANEPLUGIN_HH
//=============================================================================
