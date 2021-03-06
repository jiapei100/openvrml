// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2009  Braden McDaniel
// Copyright 2002  S. K. Bose
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 3 of the License, or (at your
// option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, see <http://www.gnu.org/licenses/>.
//

# include "cylinder_sensor.h"
# include <openvrml/node_impl_util.h>
# include <openvrml/local/float.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

namespace {

    class OPENVRML_LOCAL cylinder_sensor_node :
        public openvrml::node_impl_util::abstract_node<cylinder_sensor_node>,
        public openvrml::pointing_device_sensor_node {

        friend class openvrml_node_vrml97::cylinder_sensor_metatype;

        exposedfield<openvrml::sfbool> auto_offset_;
        exposedfield<openvrml::sffloat> disk_angle_;
        exposedfield<openvrml::sfbool> enabled_;
        exposedfield<openvrml::sffloat> max_angle_;
        exposedfield<openvrml::sffloat> min_angle_;
        exposedfield<openvrml::sffloat> offset_;
        exposedfield<openvrml::sfstring> description_;
        openvrml::sfbool is_active_;
        sfbool_emitter is_active_emitter_;
        openvrml::sfrotation rotation_changed_;
        sfrotation_emitter rotation_changed_emitter_;
        openvrml::sfvec3f track_point_changed_;
        sfvec3f_emitter track_point_changed_emitter_;
        openvrml::sfbool is_over_;
        sfbool_emitter is_over_emitter_;

        float rotation_val;
        openvrml::vec3f activationPoint;
        bool disk;
        openvrml::mat4f activationMatrix;
        openvrml::mat4f modelview;

    public:
        cylinder_sensor_node(const openvrml::node_type & type,
                             const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~cylinder_sensor_node() OPENVRML_NOTHROW;

    private:
        virtual void do_render_child(openvrml::viewer & viewer,
                                     openvrml::rendering_context context);
        virtual void do_activate(double timestamp, bool over, bool active,
                                 const double (&point)[3]);
    };

    /**
     * @class cylinder_sensor_node
     *
     * @brief CylinderSensor node instances.
     */

    /**
     * @var class cylinder_sensor_node::cylinder_sensor_metatype
     *
     * @brief Class object for CylinderSensor nodes.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<cylinder_sensor_node>::exposedfield<openvrml::sfbool> cylinder_sensor_node::auto_offset_
     *
     * @brief autoOffset exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<cylinder_sensor_node>::exposedfield<openvrml::sffloat> cylinder_sensor_node::disk_angle_
     *
     * @brief diskAngle exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<cylinder_sensor_node>::exposedfield<openvrml::sfbool> cylinder_sensor_node::enabled_
     *
     * @brief enabled exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<cylinder_sensor_node>::exposedfield<openvrml::sffloat> cylinder_sensor_node::max_angle_
     *
     * @brief maxAngle exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<cylinder_sensor_node>::exposedfield<openvrml::sffloat> cylinder_sensor_node::min_angle_
     *
     * @brief minAngle exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<cylinder_sensor_node>::exposedfield<openvrml::sffloat> cylinder_sensor_node::offset_
     *
     * @brief offset exposedField.
     */

    /**
     * @var openvrml::sfbool cylinder_sensor_node::is_active_
     *
     * @brief Active state.
     */

    /**
     * @var openvrml::sfbool_emitter cylinder_sensor_node::is_active_emitter_
     *
     * @brief isActive eventOut.
     */

    /**
     * @var openvrml::sfrotation cylinder_sensor_node::rotation_changed_
     *
     * @brief Rotation state.
     */

    /**
     * @var openvrml::sfrotation_emitter cylinder_sensor_node::rotation_changed_emitter_
     *
     * @brief rotation_changed eventOut.
     */

    /**
     * @var openvrml::sfvec3f cylinder_sensor_node::track_point_changed_
     *
     * @brief Track point state.
     */

    /**
     * @var openvrml::sfvec3f_emitter cylinder_sensor_node::track_point_changed_emitter_
     *
     * @brief trackPoint_changed eventOut.
     */

    /**
     * @var float cylinder_sensor_node::rotation_val
     *
     * @brief The rotation value.
     */

    /**
     * @var openvrml::vec3f cylinder_sensor_node::activationPoint
     *
     * @brief The activation point.
     */

    /**
     * @var bool cylinder_sensor_node::disk
     */

    /**
     * @var openvrml::mat4f cylinder_sensor_node::activationMatrix
     *
     * @brief Activation matrix.
     */

    /**
     * @var openvrml::mat4f cylinder_sensor_node::modelview
     *
     * @brief Modelview matrix.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node instance.
     * @param scope the scope to which the node belongs.
     */
    cylinder_sensor_node::
    cylinder_sensor_node(const openvrml::node_type & type,
                         const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        child_node(type, scope),
        openvrml::node_impl_util::abstract_node<cylinder_sensor_node>(type, scope),
        pointing_device_sensor_node(type, scope),
        auto_offset_(*this, true),
        disk_angle_(*this, 0.262f),
        enabled_(*this, true),
        max_angle_(*this, -1.0f),
        min_angle_(*this, 0.0f),
        offset_(*this, 0.0f),
        description_(*this),
        is_active_emitter_(*this, this->is_active_),
        rotation_changed_emitter_(*this, this->rotation_changed_),
        track_point_changed_emitter_(*this, this->track_point_changed_),
        is_over_emitter_(*this, this->is_over_),
        rotation_val(0.0f),
        disk(false),
        activationMatrix(openvrml::make_mat4f()),
        modelview(openvrml::make_mat4f())
    {
        this->node::modified(true);
    }

    /**
     * @brief Destroy.
     */
    cylinder_sensor_node::~cylinder_sensor_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Render the node.
     *
     * @param viewer    a viewer.
     * @param context   a rendering context.
     */
    void
    cylinder_sensor_node::
    do_render_child(openvrml::viewer &, openvrml::rendering_context context)
    {
        //
        // Store the modelview matrix which is calculated at the time of
        // rendering in render-context.  This matrix will be in use at the time
        // of activation.
        //
        this->modelview = context.matrix();
    }

    /**
     * @brief Called in response to user interaction.
     */
    void
    cylinder_sensor_node::do_activate(double timestamp,
                                      bool,
                                      bool active,
                                      const double (&p)[3])
    {
        using openvrml::local::pi;
        using openvrml::local::pi_2;
        using openvrml::vec3f;
        using openvrml::make_vec3f;

        if (this->enabled_.sfbool::value()) {
            // Become active
            if (active && !this->is_active_.value()) {
                this->is_active_.value(active);

                // set activation point in local coords
                vec3f v = make_vec3f(static_cast<float>(p[0]),
                                     static_cast<float>(p[1]),
                                     static_cast<float>(p[2]));
                this->activationMatrix = this->modelview.inverse();
                v *= this->activationMatrix;
                this->activationPoint = v;
                // Bearing vector in local coordinate system
                v.x(this->activationMatrix[2][0]);
                v.y(this->activationMatrix[2][1]);
                v.z(this->activationMatrix[2][2]);
                const vec3f bearing = v.normalize();
                const vec3f up = make_vec3f(0.0, 1.0, 0.0);
                double ang = acos(bearing.dot(up));
                if (ang > pi_2) { ang = pi - ang; }
                this->disk = (ang < this->disk_angle_.sffloat::value());
                // send message
                node::emit_event(this->is_active_emitter_, timestamp);
            }

            // Become inactive
            else if (!active && this->is_active_.value()) {
                this->is_active_.value(active);
                node::emit_event(this->is_active_emitter_, timestamp);

                // save auto offset of rotation
                if (this->auto_offset_.sfbool::value()) {
                    this->offset_.sffloat::value(rotation_val);
                    node::emit_event(this->offset_, timestamp);
                }
            }

            // Tracking
            else if (active) {
                using openvrml::local::fequal;

                // get local coord for touch point
                vec3f Vec = make_vec3f(static_cast<float>(p[0]),
                                       static_cast<float>(p[1]),
                                       static_cast<float>(p[2]));
                Vec = Vec * this->activationMatrix;
                this->track_point_changed_.value(Vec);
                node::emit_event(this->track_point_changed_emitter_,
                                 timestamp);
                vec3f tempv;
                float rot, radius;
                vec3f dir1 = make_vec3f(Vec[0], 0, Vec[2]);
                radius = this->disk
                    ? 1.0f
                    : dir1.length();
                dir1 = dir1.normalize();
                vec3f dir2 = make_vec3f(this->activationPoint.x(),
                                        0,
                                        this->activationPoint.z());
                dir2 = dir2.normalize();
                tempv = dir2 * dir1;
                vec3f cx(tempv);
                cx = cx.normalize();
                if (cx.length() == 0.0) { return; }
                rot = radius * float(acos(dir2.dot(dir1)));
                if (fequal(cx.y(), -1.0f)) { rot = -rot; }
                if (this->auto_offset_.sfbool::value()) {
                    rot = this->offset_.sffloat::value() + rot;
                }
                if (this->min_angle_.sffloat::value()
                    < this->max_angle_.sffloat::value()) {
                    if (rot < this->min_angle_.sffloat::value()) {
                        rot = this->min_angle_.sffloat::value();
                    } else if (rot > this->max_angle_.sffloat::value()) {
                        rot = this->max_angle_.sffloat::value();
                    }
                }
                this->rotation_val = rot;
                this->rotation_changed_.sfrotation::value(
                    openvrml::make_rotation(0, 1, 0, rot));

                node::emit_event(this->rotation_changed_emitter_, timestamp);
            }
        }
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::cylinder_sensor_metatype::id =
    "urn:X-openvrml:node:CylinderSensor";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this class object.
 */
openvrml_node_vrml97::cylinder_sensor_metatype::
cylinder_sensor_metatype(openvrml::browser & browser):
    node_metatype(cylinder_sensor_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::cylinder_sensor_metatype::~cylinder_sensor_metatype() OPENVRML_NOTHROW
{}

# define CYLINDER_SENSOR_INTERFACE_SEQ                                  \
    ((exposedfield, sfbool,     "autoOffset",         auto_offset_))    \
    ((exposedfield, sffloat,    "diskAngle",          disk_angle_))     \
    ((exposedfield, sfbool,     "enabled",            enabled_))        \
    ((exposedfield, sffloat,    "maxAngle",           max_angle_))      \
    ((exposedfield, sffloat,    "minAngle",           min_angle_))      \
    ((exposedfield, sffloat,    "offset",             offset_))         \
    ((eventout,     sfbool,     "isActive",           is_active_emitter_)) \
    ((eventout,     sfrotation, "rotation_changed",   rotation_changed_emitter_)) \
    ((eventout,     sfvec3f,    "trackPoint_changed", track_point_changed_emitter_)) \
    ((exposedfield, sfnode,     "metadata",           metadata))        \
    ((exposedfield, sfstring,   "description",        description_))    \
    ((eventout,     sfbool,     "isOver",             is_over_emitter_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_vrml97,
                                              cylinder_sensor_metatype,
                                              cylinder_sensor_node,
                                              CYLINDER_SENSOR_INTERFACE_SEQ)
