/*
 * If not stated otherwise in this file or this component's license file the
 * following copyright and licenses apply:
 *
 * Copyright 2014 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#ifndef __CTRLM_IRDB_PLUGIN_H__
#define __CTRLM_IRDB_PLUGIN_H__

#include <string>
#include <map>
#include <vector>

/** @brief Selects how the IR database is accessed. */
typedef enum {
    CTRLM_IRDB_MODE_OFFLINE,
    CTRLM_IRDB_MODE_ONLINE,
    CTRLM_IRDB_MODE_HYBRID
} ctrlm_irdb_mode_t;

/** @brief Identifies a supported infrared-controlled device type. */
typedef enum {
    CTRLM_IRDB_DEV_TYPE_TV,
    CTRLM_IRDB_DEV_TYPE_AVR,
    CTRLM_IRDB_DEV_TYPE_INVALID
} ctrlm_irdb_dev_type_t;

/** @brief Identifies an infrared command available from the database. */
typedef enum {
    CTRLM_IRDB_KEY_POWER_OFF = 0,
    CTRLM_IRDB_KEY_POWER_ON,
    CTRLM_IRDB_KEY_POWER_TOGGLE,
    CTRLM_IRDB_KEY_VOLUME_MUTE,
    CTRLM_IRDB_KEY_VOLUME_UP,
    CTRLM_IRDB_KEY_VOLUME_DOWN,
    CTRLM_IRDB_KEY_INPUT_SELECT,
    CTRLM_IRDB_KEY_INVALID,
    CTRLM_IRDB_KEY_MAX
} ctrlm_irdb_key_code_t;

/** @brief Describes an IR database vendor and the RCU support it requires. */
typedef struct {
    /** @brief Vendor name. */
    std::string   name;
    /** @brief Bitmask of RCU capabilities supported by this vendor. */
    unsigned char rcu_support_bitmask;
} ctrlm_irdb_vendor_info_t;

/** @brief List of IR database manufacturer names. */
typedef std::vector<std::string> ctrlm_irdb_manufacturer_list_t;
/** @brief List of IR database model names. */
typedef std::vector<std::string> ctrlm_irdb_model_list_t;
/** @brief List of IR database entry identifiers. */
typedef std::vector<std::string> ctrlm_irdb_entry_id_list_t;

/** @brief Maps IR commands to their raw waveform data. */
typedef std::map<ctrlm_irdb_key_code_t, std::vector<unsigned char>> ctrlm_irdb_ir_waveforms_t;

/** @brief Contains the IR waveforms for one database code set. */
typedef struct {
    /** @brief Device type represented by the code set. */
    ctrlm_irdb_dev_type_t       type;
    /** @brief Database identifier for the code set. */
    std::string                 id;
    /** @brief Waveforms indexed by command code. */
    ctrlm_irdb_ir_waveforms_t   waveforms;
} ctrlm_irdb_ir_code_set_t;

/** @brief A ranked result from an automatic IR code lookup. */
typedef struct {
    /** @brief Manufacturer associated with the result. */
    std::string manufacturer;
    /** @brief Model associated with the result. */
    std::string model;
    /** @brief Database identifier associated with the result. */
    std::string id;
    /** @brief Relative confidence or priority assigned to the result. */
    int         rank;
} ctrlm_irdb_autolookup_entry_ranked_t;

/** @brief Ranked results returned by an automatic IR code lookup. */
typedef std::vector<ctrlm_irdb_autolookup_entry_ranked_t> ctrlm_irdb_autolookup_ranked_list_t;


#ifdef __cplusplus
extern "C" {
#endif

/** @brief Returns the installed IR database version. */
std::string irdb_version();

/**
 * @brief Opens the IR database plugin.
 * @param[in] platform_tv Whether the platform itself is represented as a TV.
 * @param[in] unique_id Optional platform-specific identifier.
 * @return `true` when the plugin is opened successfully.
 */
bool ctrlm_irdb_open(bool platform_tv, const std::string &unique_id = "");

/**
 * @brief Closes the IR database plugin.
 * @return `true` when the plugin is closed successfully.
 */
bool ctrlm_irdb_close();

/**
 * @brief Initializes the IR database plugin.
 * @return `true` when initialization succeeds.
 */
bool ctrlm_irdb_initialize();

/**
 * @brief Returns information about all installed IR database vendors.
 * @param[out] info Receives the installed vendor information.
 * @return `true` when the vendor information is retrieved successfully.
 */
bool ctrlm_irdb_get_supported_vendor_info(std::vector<ctrlm_irdb_vendor_info_t> &info);

/**
 * @brief Sets the IR database vendor supported by the RCU.
 * @param[in] vendor Vendor information and RCU support mask.
 * @return `true` when the preferred vendor is set successfully.
 */
bool ctrlm_irdb_set_preferred_vendor(const ctrlm_irdb_vendor_info_t &vendor);

/**
 * @brief Returns information about the currently selected vendor.
 * @param[out] info Receives the selected vendor information.
 * @return `true` when the vendor information is retrieved successfully.
 */
bool ctrlm_irdb_get_vendor_info(ctrlm_irdb_vendor_info_t &info);

/**
 * @brief Lists manufacturers matching a device type and prefix.
 * @param[out] manufacturers Receives matching manufacturer names.
 * @param[in] type Device type to search.
 * @param[in] prefix Optional manufacturer-name prefix.
 * @return `true` when the list is retrieved successfully.
 */
bool ctrlm_irdb_get_manufacturers(ctrlm_irdb_manufacturer_list_t &manufacturers, ctrlm_irdb_dev_type_t type, const std::string &prefix);

/**
 * @brief Lists models matching a manufacturer, device type, and prefix.
 * @param[out] models Receives matching model names.
 * @param[in] type Device type to search.
 * @param[in] manufacturer Manufacturer to search.
 * @param[in] prefix Optional model-name prefix.
 * @return `true` when the list is retrieved successfully.
 */
bool ctrlm_irdb_get_models(ctrlm_irdb_model_list_t &models, ctrlm_irdb_dev_type_t type, const std::string &manufacturer, const std::string &prefix);

/**
 * @brief Lists code-set identifiers for a manufacturer and model.
 * @param[out] ids Receives matching entry identifiers.
 * @param[in] type Device type to search.
 * @param[in] manufacturer Manufacturer to search.
 * @param[in] model Model to search.
 * @return `true` when the list is retrieved successfully.
 */
bool ctrlm_irdb_get_entry_ids(ctrlm_irdb_entry_id_list_t &ids, ctrlm_irdb_dev_type_t type, const std::string &manufacturer, const std::string &model);

/**
 * @brief Retrieves an IR code set by device type and database identifier.
 * @param[out] code_set Receives the code set.
 * @param[in] type Device type of the code set.
 * @param[in] id Database identifier of the code set.
 * @return `true` when the code set is retrieved successfully.
 */
bool ctrlm_irdb_get_ir_code_set(ctrlm_irdb_ir_code_set_t &code_set, ctrlm_irdb_dev_type_t type, const std::string &id);

/**
 * @brief Finds ranked IR code sets using an HDMI InfoFrame.
 * @param[out] codes Receives ranked matching code sets.
 * @param[out] type Receives the matched device type.
 * @param[in] infoframe InfoFrame data to match.
 * @param[in] infoframe_len Length of `infoframe` in bytes.
 * @return `true` when lookup succeeds.
 */
bool ctrlm_irdb_get_ir_codes_by_infoframe(ctrlm_irdb_autolookup_ranked_list_t &codes, ctrlm_irdb_dev_type_t &type, unsigned char *infoframe, unsigned int infoframe_len);

/**
 * @brief Finds ranked IR code sets using EDID data.
 * @param[out] codes Receives ranked matching code sets.
 * @param[out] type Receives the matched device type.
 * @param[in] edid EDID data to match.
 * @param[in] edid_len Length of `edid` in bytes.
 * @return `true` when lookup succeeds.
 */
bool ctrlm_irdb_get_ir_codes_by_edid(ctrlm_irdb_autolookup_ranked_list_t &codes, ctrlm_irdb_dev_type_t &type, unsigned char *edid, unsigned int edid_len);

/**
 * @brief Finds ranked IR code sets using HDMI-CEC device information.
 * @param[out] codes Receives ranked matching code sets.
 * @param[out] type Receives the matched device type.
 * @param[in] osd CEC on-screen display name.
 * @param[in] vendor_id CEC vendor identifier.
 * @param[in] logical_address CEC logical address.
 * @return `true` when lookup succeeds.
 */
bool ctrlm_irdb_get_ir_codes_by_cec(ctrlm_irdb_autolookup_ranked_list_t &codes, ctrlm_irdb_dev_type_t &type, const std::string &osd, unsigned int vendor_id, unsigned int logical_address);

#ifdef __cplusplus
}
#endif

#endif