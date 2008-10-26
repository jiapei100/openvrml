// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2006, 2007, 2008  Braden McDaniel
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

# include "h_anim_humanoid.h"
# include <openvrml/node_impl_util.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

using namespace openvrml;
using namespace openvrml::node_impl_util;
using namespace std;

namespace {

    /**
     * @brief Represents HAnimHumanoid node instances.
     */
    class OPENVRML_LOCAL h_anim_humanoid_node :
        public abstract_node<h_anim_humanoid_node>,
        public child_node {

        friend class openvrml_node_x3d_h_anim::h_anim_humanoid_metatype;

        exposedfield<sfvec3f> center_;
        exposedfield<mfstring> info_;
        exposedfield<mfnode> joints_;
        exposedfield<sfstring> name_;
        exposedfield<sfrotation> rotation_;
        exposedfield<sfvec3f> scale_;
        exposedfield<sfrotation> scale_orientation_;
        exposedfield<mfnode> segments_;
        exposedfield<mfnode> sites_;
        exposedfield<mfnode> skeleton_;
        exposedfield<mfnode> skin_;
        exposedfield<sfnode> skin_coord_;
        exposedfield<sfnode> skin_normal_;
        exposedfield<sfvec3f> translation_;
        exposedfield<sfstring> version_;
        exposedfield<mfnode> viewpoints_;
        sfvec3f bbox_center_;
        sfvec3f bbox_size_;

    public:
        h_anim_humanoid_node(const node_type & type,
                            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~h_anim_humanoid_node() OPENVRML_NOTHROW;
    };


    /**
     * @var h_anim_humanoid_node::h_anim_humanoid_metatype
     *
     * @brief Class object for HAnimHumanoid nodes.
     */

    /**
     * @var h_anim_humanoid_node::center_
     *
     * @brief center exposedField
     */

    /**
     * @var h_anim_humanoid_node::info_
     *
     * @brief info exposedField
     */

    /**
     * @var h_anim_humanoid_node::joints_
     *
     * @brief joints exposedField
     */

    /**
     * @var h_anim_humanoid_node::name_
     *
     * @brief name exposedField
     */

    /**
     * @var h_anim_humanoid_node::rotation_
     *
     * @brief rotation exposedField
     */

    /**
     * @var h_anim_humanoid_node::scale_
     *
     * @brief scale exposedField
     */

    /**
     * @var h_anim_humanoid_node::scale_orientation_
     *
     * @brief scale_orientation exposedField
     */

    /**
     * @var h_anim_humanoid_node::segments_
     *
     * @brief segments exposedField
     */

    /**
     * @var h_anim_humanoid_node::sites_
     *
     * @brief sites exposedField
     */

    /**
     * @var h_anim_humanoid_node::skeleton_
     *
     * @brief skeleton exposedField
     */

    /**
     * @var h_anim_humanoid_node::skin_
     *
     * @brief skin exposedField
     */

    /**
     * @var h_anim_humanoid_node::skin_coord_
     *
     * @brief skin_coord exposedField
     */

    /**
     * @var h_anim_humanoid_node::skin_normal_
     *
     * @brief skin_normal exposedField
     */

    /**
     * @var h_anim_humanoid_node::translation_
     *
     * @brief translation exposedField
     */

    /**
     * @var h_anim_humanoid_node::version_
     *
     * @brief version exposedField
     */

    /**
     * @var h_anim_humanoid_node::viewpoints_
     *
     * @brief viewpoints exposedField
     */

    /**
     * @var h_anim_humanoid_node::bbox_center_
     *
     * @brief bbox_center field
     */

    /**
     * @var h_anim_humanoid_node::bbox_size_
     *
     * @brief bbox_size field
     */


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope the scope to which the node belongs.
     */
    h_anim_humanoid_node::
    h_anim_humanoid_node(const node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        center_(*this),
        info_(*this),
        joints_(*this),
        name_(*this),
        rotation_(*this),
        scale_(*this, make_vec3f(1.0, 1.0, 1.0)),
        scale_orientation_(*this),
        segments_(*this),
        sites_(*this),
        skeleton_(*this),
        skin_(*this),
        skin_coord_(*this),
        skin_normal_(*this),
        translation_(*this),
        version_(*this),
        viewpoints_(*this),
        bbox_size_(make_vec3f(-1.0f, -1.0f, -1.0f))
    {}

    /**
     * @brief Destroy.
     */
    h_anim_humanoid_node::~h_anim_humanoid_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_x3d_h_anim::h_anim_humanoid_metatype::id =
    "urn:X-openvrml:node:HAnimHumanoid";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this
 *                @c h_anim_humanoid_metatype.
 */
openvrml_node_x3d_h_anim::h_anim_humanoid_metatype::
h_anim_humanoid_metatype(openvrml::browser & browser):
    node_metatype(h_anim_humanoid_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_h_anim::h_anim_humanoid_metatype::~h_anim_humanoid_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a @c node_type.
 *
 * @param id            the name for the new @c node_type.
 * @param interfaces    the interfaces for the new @c node_type.
 *
 * @return a @c node_type capable of creating HAnimHumanoid nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface
 *                                  not supported by
 *                                  @c h_anim_humanoid_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_x3d_h_anim::h_anim_humanoid_metatype::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
{
    typedef boost::array<node_interface, 19> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "center"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfstring_id,
                       "info"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfnode_id,
                       "joints"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "name"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfrotation_id,
                       "rotation"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "scale"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfrotation_id,
                       "scaleOrientation"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfnode_id,
                       "segments"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfnode_id,
                       "sites"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfnode_id,
                       "skeleton"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfnode_id,
                       "skin"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "skinCoord"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "skinNormal"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "translation"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "version"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfnode_id,
                       "viewpoints"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxCenter"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxSize")
    };
    typedef node_type_impl<h_anim_humanoid_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & the_node_type = static_cast<node_type_t &>(*type);

    for (node_interface_set::const_iterator interface_(interfaces.begin());
         interface_ != interfaces.end();
         ++interface_) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &h_anim_humanoid_node::metadata);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &h_anim_humanoid_node::center_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &h_anim_humanoid_node::info_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &h_anim_humanoid_node::joints_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &h_anim_humanoid_node::name_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &h_anim_humanoid_node::rotation_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &h_anim_humanoid_node::scale_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &h_anim_humanoid_node::scale_orientation_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &h_anim_humanoid_node::segments_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &h_anim_humanoid_node::sites_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &h_anim_humanoid_node::skeleton_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &h_anim_humanoid_node::skin_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &h_anim_humanoid_node::skin_coord_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &h_anim_humanoid_node::skin_normal_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &h_anim_humanoid_node::translation_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &h_anim_humanoid_node::version_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &h_anim_humanoid_node::viewpoints_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &h_anim_humanoid_node::bbox_center_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &h_anim_humanoid_node::bbox_size_);
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}