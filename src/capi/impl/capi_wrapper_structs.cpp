/* Copyright © 2018 Apple Inc. All rights reserved.
 *
 * Use of this source code is governed by a BSD-3-clause license that can
 * be found in the LICENSE.txt file or at https://opensource.org/licenses/BSD-3-Clause
 */
#include <capi/impl/capi_wrapper_structs.hpp>

#define DEFINE_CAPI_WRAPPER_STRUCT_TYPE_INFO(struct_name) \
  capi_struct_type_info_##struct_name capi_struct_type_info_##struct_name##_inst

DEFINE_CAPI_WRAPPER_STRUCT_TYPE_INFO(tc_error);
DEFINE_CAPI_WRAPPER_STRUCT_TYPE_INFO(tc_datetime);
DEFINE_CAPI_WRAPPER_STRUCT_TYPE_INFO(tc_flex_dict);
DEFINE_CAPI_WRAPPER_STRUCT_TYPE_INFO(tc_flex_list);
DEFINE_CAPI_WRAPPER_STRUCT_TYPE_INFO(tc_ndarray);
DEFINE_CAPI_WRAPPER_STRUCT_TYPE_INFO(tc_flexible_type);
DEFINE_CAPI_WRAPPER_STRUCT_TYPE_INFO(tc_flex_enum_list);
DEFINE_CAPI_WRAPPER_STRUCT_TYPE_INFO(tc_sarray);
DEFINE_CAPI_WRAPPER_STRUCT_TYPE_INFO(tc_sframe);
DEFINE_CAPI_WRAPPER_STRUCT_TYPE_INFO(tc_variant);
DEFINE_CAPI_WRAPPER_STRUCT_TYPE_INFO(tc_parameters);
DEFINE_CAPI_WRAPPER_STRUCT_TYPE_INFO(tc_model);
DEFINE_CAPI_WRAPPER_STRUCT_TYPE_INFO(tc_groupby_aggregator);
DEFINE_CAPI_WRAPPER_STRUCT_TYPE_INFO(tc_plot);
