#include "PlanePlugin.hh"

#include <iostream>

#include <OpenFlipper/BasePlugin/PluginFunctions.hh>
#include <OpenFlipper/common/GlobalOptions.hh>

#include <ACG/Scenegraph/LineNode.hh>

const char *PLANE = "CreatePlaneNode";
const char *PLANE_RESIZE = "PlaneResize";

//------------------------------------------------------------------------------

/** \brief Constructor
 *
 */
PlanePlugin::PlanePlugin() {}

/*******************************************************************************
        BaseInterface implementation
 *******************************************************************************/

/** \brief Initialize the plugin
 *
 */
void PlanePlugin::initializePlugin() {
  // register keys
  emit registerKey(Qt::Key_Escape, Qt::NoModifier, "Cancel Plane", true);
}

//------------------------------------------------------------------------------

/** \brief Second initialization phase
 *
 */
void PlanePlugin::pluginsInitialized() {
  setDescriptions();

  emit addPickMode(PLANE);

  emit addPickMode(PLANE_RESIZE);

  createPlane_ =
      new QtPlaneSelect(PluginFunctions::viewerProperties().glState());
  connect(createPlane_, SIGNAL(signalTriggerCut()), this,
          SLOT(slotCreatePlaneTriggered()));
  connect(createPlane_, SIGNAL(updateViewProxy()), this, SIGNAL(updateView()));

  // Add a Toolbar
  QToolBar *toolbar = new QToolBar("Plane Toolbar");
  // createPlane_Node
  planeNodeAction_ = new QAction(tr("&Create Plane"), this);
  planeNodeAction_->setCheckable(true);
  planeNodeAction_->setStatusTip(tr("Create a Plane Node"));
  planeNodeAction_->setIcon(QIcon(OpenFlipper::Options::iconDirStr() +
                                  OpenFlipper::Options::dirSeparator() +
                                  "plane_plane.png"));
  connect(planeNodeAction_, SIGNAL(triggered()), this,
          SLOT(slotCreatePlaneNode()));
  toolbar->addAction(planeNodeAction_);

  emit addToolbar(toolbar);
}

/*******************************************************************************
    PickingInterface implementation
 *******************************************************************************/

/** \brief the pickMode changed
 *
 * @param _mode the new pickMode
 */
void PlanePlugin::slotPickModeChanged(const std::string &_mode) {
  planeNodeAction_->setChecked(_mode == PLANE);
}

/*******************************************************************************
KeyInterface implementation
*******************************************************************************/

/** \brief a keyRelease event occured
 *
 * @param _event the event that occured
 */
void PlanePlugin::slotKeyReleaseEvent(QKeyEvent *_event) {
  if (PluginFunctions::pickMode() == PLANE)
    createPlane_->slotKeyReleaseEvent(_event);
}

/*******************************************************************************
    MouseInterface implementation
 *******************************************************************************/

/** \brief a mouse event occured
 *
 * @param _event the event that occured
 */
void PlanePlugin::slotMouseEvent(QMouseEvent *_event) {

  //  auto determinant = [](ACG::Vec3f const &c1, ACG::Vec3f const &c2,
  //                        ACG::Vec3f const &c3) -> float {
  //    return (c1[0] * c2[1] * c3[2]) + (c2[0] * c3[1] * c1[2]) +
  //           (c3[0] * c1[1] * c2[2]) - (c3[0] * c2[1] * c1[2]) -
  //           (c2[0] * c1[1] * c3[2]) - (c1[0] * c3[1] * c2[2]);
  //  };
  /// only works in constrained cases
  //  auto rayPlaneIntersection =
  //      [&determinant](ACG::Vec3f const &s1, ACG::Vec3f const &s2,
  //                     ACG::Vec3f const &p, ACG::Vec3f const &q,
  //                     ACG::Vec3f const &r, float &alpha, float &beta) {
  //        /// implementation of cramers rule to compute closed form
  //        solution for
  //        /// plane ray intersections (closed form solution) problematic
  //        if ray
  //        /// points away or converges to parallel to plane
  //        /// s1 and s2 are two points that define the ray (origin &
  //        origin+dir)
  //        /// p,q,r span a triangle that defines the plane
  //        /// alpha & beta are return params for the barycentric
  //        coordinates
  //        /// alpha belongs to p, beta belongs to q
  //        const auto denom = determinant(s1, r - p, r - q);
  //        alpha = determinant(s1, r - s2, r - q) / denom;
  //        beta = determinant(s1, r - p, r - s2) / denom;
  //      };

  /// modification of triangleIntersection
  auto rayPlaneIntersection = [](ACG::Vec3d const &origin,
                                 ACG::Vec3d const &dir, ACG::Vec3d const &p,
                                 ACG::Vec3d const &q, ACG::Vec3d const &r,
                                 double &alpha, double &beta) -> bool {
    {
      ACG::Vec3d edge1, edge2, tvec, pvec, qvec;
      double det, inv_det;

      edge1 = q - p;
      edge2 = r - p;
      pvec = dir % edge2;
      det = edge1 | pvec;

      constexpr double EPSILON = std::numeric_limits<double>::epsilon() * 1e2;
      if (det > -EPSILON && det < EPSILON) {
        std::cerr << "det within eps!" << std::endl;
        return false;
      }
      inv_det = 1.f / det;
      tvec = origin - p;
      alpha = (tvec | pvec) * inv_det;
      qvec = tvec % edge1;
      beta = (dir | qvec) * inv_det;

      /// we don't really care about legal barycentric coordinates, however if
      /// you really want to know if you are inside the defining triangle,
      /// there you go
      if (alpha < 0.0 || alpha > 1.0)
        return false;
      if (beta < 0.0 || alpha + beta > 1.0)
        return false;

      return true;
    }
  };

  if (PluginFunctions::pickMode() == PLANE)
    createPlane_->slotMouseEvent(_event);
  if (PluginFunctions::pickMode() == PLANE_RESIZE) {
    switch (_event->type()) {
    case QEvent::MouseButtonPress: {
      ACG::Vec3d sourcePoint3D;
      size_t node_idx, target_idx;
      if (PluginFunctions::scenegraphPick(ACG::SceneGraph::PICK_ANYTHING,
                                          _event->pos(), node_idx, target_idx,
                                          &sourcePoint3D)) {
        BaseObjectData *obj = nullptr;
        if (PluginFunctions::getPickedObject(node_idx, obj)) {
          // is picked object Plane?
          curPlane_ = PluginFunctions::planeObject(obj);
          lastObjId_ = obj->id();

          if (curPlane_) {
            origPlane_ = curPlane_->plane();
            QPoint position = _event->pos();

            /// obtain the ray for intersecting with the quad/plane
            ACG::Vec3d origin;
            PluginFunctions::viewerProperties().glState().viewing_ray(
                position.x(),
                PluginFunctions::viewerProperties().glState().context_height() -
                    position.y(),
                origin, viewDirection_);

            /// compute intersection
            {
              const auto center = curPlane_->plane().position;
              const auto xdir = curPlane_->plane().xDirection / 2.;
              const auto ydir = curPlane_->plane().yDirection / 2.;
              const auto p0 = center + xdir + ydir;
              const auto p1 = center + xdir - ydir;
              const auto p2 = center - xdir - ydir;
              double u = -1, v = -1, w = -1;
              rayPlaneIntersection(origin, viewDirection_, p0, p1, p2, u, v);
              w = 1 - (u + v);
              wMouseDownPosition_ = (w * p0) + (u * p1) + (v * p2);
            }

            pickedCorner_ = target_idx;

            // We hit a corner so start dragging
            dragging_ = true;
          }
        }
      }

      break;
    }

    case QEvent::MouseMove: {

      if (dragging_ && curPlane_) {

        QPoint position = _event->pos();

        /// matrix to transform points from image plane to curPlane_
        auto plane = origPlane_;

        /// get the world position of the projected cursor by intersecting
        /// plane equation with ray
        {
          /// view coords in worldspace, with flipped y-axis
          const auto viewCoord = ACG::Vec3d(
              position.x(),
              PluginFunctions::viewerProperties().glState().context_height() -
                  position.y(),
              .5);

          ACG::Vec3d origin;
          ACG::Vec3d dir;
          PluginFunctions::viewerProperties().glState().viewing_ray(
              viewCoord[0], viewCoord[1], origin, dir);
          double u = -3.1415926539, v = -3.1415926539;
          const auto center = plane.position;
          const auto xdir = plane.xDirection / 2.;
          const auto ydir = plane.yDirection / 2.;
          const auto p0 = ACG::Vec3d{center + xdir + ydir};
          const auto p1 = ACG::Vec3d{center + xdir - ydir};
          const auto p2 = ACG::Vec3d{center - xdir - ydir};
          /// we can assume that most of the time, there wont be an
          /// intersection. but as we only check one half of the triangle, the
          /// other will always be outside

          /// we dont really care about the result here
          rayPlaneIntersection(origin, dir, p0, p1, p2, u, v);
          const auto w = 1. - (u + v);
          wCurrMousePos_ = (w * p0) + (u * p1) + (v * p2);
        }

        /// relative to plane
        const auto wMouseDownPosition = wMouseDownPosition_ - plane.position;
        /// relative to plane
        const auto wCurrMousePos = wCurrMousePos_ - plane.position;

        /// update vector in worldspace
        const auto worldSpaceUpdate = (wCurrMousePos - wMouseDownPosition) / 2.;

        if (pickedCorner_ != 0) {
          /// We are actually dragging a corner

          const auto distClickOrigin = wMouseDownPosition;

          /// scale by half updates length
          auto scale = (worldSpaceUpdate + distClickOrigin) / distClickOrigin;
          scale[2] = 1; /// we dont do anything in z-dir

          const auto xscale = scale[0] * plane.xDirection.length();
          const auto yscale = scale[1] * plane.yDirection.length();
          plane.setSize(xscale, yscale);
        }

        /// we are just dragging the plane along (also happens when scaling,
        /// dont worry)
        plane.position += worldSpaceUpdate;

        curPlane_->plane() = plane; /// overwrite plane

        /// signal for updates
        if (lastObjId_ > 0)
          emit updatedObject(lastObjId_, UPDATE_GEOMETRY);
        else
          emit log(LOGERR, tr("COULD NOT UPDATE OBJECT"));
      }

      break;
    }
    case QEvent::MouseButtonRelease: {

      // Stop dragging operation reset all properties
      dragging_ = false;
      pickedCorner_ = 0;
      wMouseDownPosition_ = ACG::Vec3d{.0};
      curPlane_ = nullptr;
      lastObjId_ = -1;

      break;
    }
    default:
      break;
    }
  }
}

/*******************************************************************************
         PlanePlugin Implementation
 *******************************************************************************/

//------------------------------------------------------------------------------

/** \brief Plane Node Button
 *
 */
void PlanePlugin::slotCreatePlaneNode() {
  PluginFunctions::actionMode(Viewer::PickingMode);
  PluginFunctions::pickMode(PLANE);
}

//------------------------------------------------------------------------------

void PlanePlugin::slotCreatePlaneTriggered() {
  // get object
  BaseObjectData *obj;

  if (PluginFunctions::getPickedObject(createPlane_->getNode(), obj)) {
    if (obj == nullptr) {
      emit log(LOGERR, "Unable to get object");
      return;
    }

    // generate a plane object
    int planeId = -1;

    // add new plane
    emit addEmptyObject(DATA_PLANE, planeId);

    // get current planeobject
    BaseObjectData *planeObj;
    PluginFunctions::getObject(planeId, planeObj);

    // get plane object
    PlaneObject *currentPlane = PluginFunctions::planeObject(planeObj);

    ACG::Vec3d point = createPlane_->getSourcePoint();
    ACG::Vec3d normal = createPlane_->getNormal();

    ACG::Vec3d center;
    double radius;

    if (getIntersectionParams(*obj, center, radius)) {
      radius = 1.5 * radius;
      radius = std::min(radius, PluginFunctions::sceneRadius());

      currentPlane->plane().setPlane(center, normal);
      currentPlane->plane().setSize(2.0 * radius, 2.0 * radius);

    } else {
      std::cerr << "unable to get intersection params" << std::endl;
      currentPlane->plane().setPlane(point, normal);
      currentPlane->plane().setSize(PluginFunctions::sceneRadius(),
                                    PluginFunctions::sceneRadius());
    }

    currentPlane->planeNode()->show();

    emit updatedObject(planeId, UPDATE_ALL);
  }
}

//------------------------------------------------------------------------------

bool PlanePlugin::getIntersectionParams(BaseObjectData &_obj,
                                        ACG::Vec3d &_center, double &_radius) {
  ACG::Vec3d point = createPlane_->getSourcePoint();
  ACG::Vec3d normal = createPlane_->getNormal();

  // get the intersection points
  std::vector<ACG::Vec3d> linePoints;
  bool closed = false;
  if (_obj.dataType(DATA_TRIANGLE_MESH)) {
    TriMesh *mesh = PluginFunctions::triMesh(&_obj);

    if (mesh == nullptr)
      return false;

    // get a edge of the mesh that is cut by the plane
    TriMesh::EdgeHandle eh = getCuttedEdge(*mesh, normal, point);

    if (!eh.is_valid())
      return false;

    TriMesh::FaceHandle fh = mesh->face_handle(mesh->halfedge_handle(eh, 0));

    if (!fh.is_valid())
      fh = mesh->face_handle(mesh->halfedge_handle(eh, 1));

    // get all intersection points
    linePoints = getIntersectionPoints(mesh, fh.idx(), normal, point, closed);

  } else {
    PolyMesh *mesh = PluginFunctions::polyMesh(&_obj);

    if (mesh == nullptr)
      return false;

    // get a edge of the mesh that is cut by the plane
    PolyMesh::EdgeHandle eh = getCuttedEdge(*mesh, normal, point);

    if (!eh.is_valid())
      return false;

    PolyMesh::FaceHandle fh = mesh->face_handle(mesh->halfedge_handle(eh, 0));

    if (!fh.is_valid())
      fh = mesh->face_handle(mesh->halfedge_handle(eh, 1));

    // get all intersection points
    linePoints = getIntersectionPoints(mesh, fh.idx(), normal, point, closed);
  }

  if (linePoints.empty())
    return false;

  _center = ACG::Vec3d(0.0, 0.0, 0.0);

  for (uint i = 0; i < linePoints.size(); i++)
    _center += linePoints[i];

  _center /= (double)linePoints.size();

  _radius = 0;

  for (uint i = 0; i < linePoints.size(); i++) {
    double dist = (_center - linePoints[i]).norm();

    if (dist > _radius)
      _radius = dist;
  }
  return true;
}

//------------------------------------------------------------------------------
