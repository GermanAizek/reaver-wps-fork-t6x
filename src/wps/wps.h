/*
 * Wi-Fi Protected Setup
 * Copyright (c) 2007-2009, Jouni Malinen <j@w1.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#ifndef WPS_H
#define WPS_H

#include "wps_defs.h"

/**
 * enum wsc_op_code - EAP-WSC OP-Code values
 */
enum wsc_op_code {
	WSC_UPnP = 0 /* No OP Code in UPnP transport */,
	WSC_Start = 0x01,
	WSC_ACK = 0x02,
	WSC_NACK = 0x03,
	WSC_MSG = 0x04,
	WSC_Done = 0x05,
	WSC_FRAG_ACK = 0x06
};

struct wps_registrar;
struct upnp_wps_device_sm;
struct wps_er;

/**
 * struct wps_credential - WPS Credential
 * @ssid: SSID
 * @ssid_len: Length of SSID
 * @auth_type: Authentication Type (WPS_AUTH_OPEN, .. flags)
 * @encr_type: Encryption Type (WPS_ENCR_NONE, .. flags)
 * @key_idx: Key index
 * @key: Key
 * @key_len: Key length in octets
 * @mac_addr: MAC address of the Credential receiver
 * @cred_attr: Unparsed Credential attribute data (used only in cred_cb());
 *	this may be %NULL, if not used
 * @cred_attr_len: Length of cred_attr in octets
 */
struct wps_credential {
	char ssid[32];
	size_t ssid_len;
	u16 auth_type;
	u16 encr_type;
	u8 key_idx;
	u8 key[64];
	size_t key_len;
	u8 mac_addr[ETH_ALEN];
	const u8 *cred_attr;
	size_t cred_attr_len;
};

#define WPS_DEV_TYPE_LEN 8
#define WPS_DEV_TYPE_BUFSIZE 21

/**
 * struct wps_device_data - WPS Device Data
 * @mac_addr: Device MAC address
 * @device_name: Device Name (0..32 octets encoded in UTF-8)
 * @manufacturer: Manufacturer (0..64 octets encoded in UTF-8)
 * @model_name: Model Name (0..32 octets encoded in UTF-8)
 * @model_number: Model Number (0..32 octets encoded in UTF-8)
 * @serial_number: Serial Number (0..32 octets encoded in UTF-8)
 * @pri_dev_type: Primary Device Type
 * @os_version: OS Version
 * @rf_bands: RF bands (WPS_RF_24GHZ, WPS_RF_50GHZ flags)
 */
struct wps_device_data {
	u8 mac_addr[ETH_ALEN];
	char *device_name;
	char *manufacturer;
	char *model_name;
	char *model_number;
	char *serial_number;
	u8 pri_dev_type[WPS_DEV_TYPE_LEN];
	u32 os_version;
	u8 rf_bands;
};

struct oob_conf_data {
	enum {
		OOB_METHOD_UNKNOWN = 0,
		OOB_METHOD_DEV_PWD_E,
		OOB_METHOD_DEV_PWD_R,
		OOB_METHOD_CRED
	} oob_method;
	struct wpabuf *dev_password;
	struct wpabuf *pubkey_hash;
};

/**
 * struct wps_config - WPS configuration for a single registration protocol run
 */
struct wps_config {
	/**
	 * wps - Pointer to long term WPS context
	 */
	struct wps_context *wps;

	/**
	 * assoc_wps_ie: (Re)AssocReq WPS IE (in AP; %NULL if not AP)
	 */
	const struct wpabuf *assoc_wps_ie;

	/**
	 * pin - Enrollee Device Password (%NULL for Registrar or PBC)
	 */
	const u8 *pin;

	/**
	 * pin_len - Length on pin in octets
	 */
	size_t pin_len;

	/**
	 * registrar - Whether this end is a Registrar
	 */
	int registrar;

	/**
	 * pbc - Whether this is protocol run uses PBC
	 */
	int pbc;

	/**
	 * new_ap_settings - New AP settings (%NULL if not used)
	 *
	 * This parameter provides new AP settings when using a wireless
	 * stations as a Registrar to configure the AP. %NULL means that AP
	 * will not be reconfigured, i.e., the station will only learn the
	 * current AP settings by using AP PIN.
	 */
	const struct wps_credential *new_ap_settings;

	/**
	 * peer_addr: MAC address of the peer in AP; %NULL if not AP
	 */
	const u8 *peer_addr;

	/**
	 * use_psk_key - Use PSK format key in Credential
	 *
	 * Force PSK format to be used instead of ASCII passphrase when
	 * building Credential for an Enrollee. The PSK value is set in
	 * struct wpa_context::psk.
	 */
	int use_psk_key;
};

struct wps_data * wps_init(const struct wps_config *cfg);

void wps_deinit(struct wps_data *data);

/**
 * enum wps_process_res - WPS message processing result
 */
enum wps_process_res {
	/**
	 * WPS_DONE - Processing done
	 */
	WPS_DONE,

	/**
	 * WPS_CONTINUE - Processing continues
	 */
	WPS_CONTINUE,

	/**
	 * WPS_FAILURE - Processing failed
	 */
	WPS_FAILURE,

	/**
	 * WPS_PENDING - Processing continues, but waiting for an external
	 *	event (e.g., UPnP message from an external Registrar)
	 */
	WPS_PENDING
};
enum wps_process_res wps_process_msg(struct wps_data *wps,
				     enum wsc_op_code op_code,
				     const struct wpabuf *msg);

struct wpabuf * wps_get_msg(struct wps_data *wps, enum wsc_op_code *op_code);

int wps_is_selected_pbc_registrar(const struct wpabuf *msg);
int wps_is_selected_pin_registrar(const struct wpabuf *msg);
const u8 * wps_get_uuid_e(const struct wpabuf *msg);

struct wpabuf * wps_build_assoc_req_ie(enum wps_request_type req_type);
struct wpabuf * wps_build_assoc_resp_ie(void);
struct wpabuf * wps_build_probe_req_ie(int pbc, struct wps_device_data *dev,
				       const u8 *uuid,
				       enum wps_request_type req_type);


/**
 * struct wps_registrar_config - WPS Registrar configuration
 */
struct wps_registrar_config {
	/**
	 * new_psk_cb - Callback for new PSK
	 * @ctx: Higher layer context data (cb_ctx)
	 * @mac_addr: MAC address of the Enrollee
	 * @psk: The new PSK
	 * @psk_len: The length of psk in octets
	 * Returns: 0 on success, -1 on failure
	 *
	 * This callback is called when a new per-device PSK is provisioned.
	 */
	int (*new_psk_cb)(void *ctx, const u8 *mac_addr, const u8 *psk,
			  size_t psk_len);

	/**
	 * set_ie_cb - Callback for WPS IE changes
	 * @ctx: Higher layer context data (cb_ctx)
	 * @beacon_ie: WPS IE for Beacon
	 * @probe_resp_ie: WPS IE for Probe Response
	 * Returns: 0 on success, -1 on failure
	 *
	 * This callback is called whenever the WPS IE in Beacon or Probe
	 * Response frames needs to be changed (AP only). Callee is responsible
	 * for freeing the buffers.
	 */
	int (*set_ie_cb)(void *ctx, struct wpabuf *beacon_ie,
			 struct wpabuf *probe_resp_ie);

	/**
	 * pin_needed_cb - Callback for requesting a PIN
	 * @ctx: Higher layer context data (cb_ctx)
	 * @uuid_e: UUID-E of the unknown Enrollee
	 * @dev: Device Data from the unknown Enrollee
	 *
	 * This callback is called whenever an unknown Enrollee requests to use
	 * PIN method and a matching PIN (Device Password) is not found in
	 * Registrar data.
	 */
	void (*pin_needed_cb)(void *ctx, const u8 *uuid_e,
			      const struct wps_device_data *dev);

	/**
	 * reg_success_cb - Callback for reporting successful registration
	 * @ctx: Higher layer context data (cb_ctx)
	 * @mac_addr: MAC address of the Enrollee
	 * @uuid_e: UUID-E of the Enrollee
	 *
	 * This callback is called whenever an Enrollee completes registration
	 * successfully.
	 */
	void (*reg_success_cb)(void *ctx, const u8 *mac_addr,
			       const u8 *uuid_e);

	/**
	 * set_sel_reg_cb - Callback for reporting selected registrar changes
	 * @ctx: Higher layer context data (cb_ctx)
	 * @sel_reg: Whether the Registrar is selected
	 * @dev_passwd_id: Device Password ID to indicate with method or
	 *	specific password the Registrar intends to use
	 * @sel_reg_config_methods: Bit field of active config methods
	 *
	 * This callback is called whenever the Selected Registrar state
	 * changes (e.g., a new PIN becomes available or PBC is invoked). This
	 * callback is only used by External Registrar implementation;
	 * set_ie_cb() is used by AP implementation in similar caes, but it
	 * provides the full WPS IE data instead of just the minimal Registrar
	 * state information.
	 */
	void (*set_sel_reg_cb)(void *ctx, int sel_reg, u16 dev_passwd_id,
			       u16 sel_reg_config_methods);

	/**
	 * enrollee_seen_cb - Callback for reporting Enrollee based on ProbeReq
	 * @ctx: Higher layer context data (cb_ctx)
	 * @addr: MAC address of the Enrollee
	 * @uuid_e: UUID of the Enrollee
	 * @pri_dev_type: Primary device type
	 * @config_methods: Config Methods
	 * @dev_password_id: Device Password ID
	 * @request_type: Request Type
	 * @dev_name: Device Name (if available)
	 */
	void (*enrollee_seen_cb)(void *ctx, const u8 *addr, const u8 *uuid_e,
				 const u8 *pri_dev_type, u16 config_methods,
				 u16 dev_password_id, u8 request_type,
				 const char *dev_name);

	/**
	 * cb_ctx: Higher layer context data for Registrar callbacks
	 */
	void *cb_ctx;

	/**
	 * skip_cred_build: Do not build credential
	 *
	 * This option can be used to disable internal code that builds
	 * Credential attribute into M8 based on the current network
	 * configuration and Enrollee capabilities. The extra_cred data will
	 * then be used as the Credential(s).
	 */
	int skip_cred_build;

	/**
	 * extra_cred: Additional Credential attribute(s)
	 *
	 * This optional data (set to %NULL to disable) can be used to add
	 * Credential attribute(s) for other networks into M8. If
	 * skip_cred_build is set, this will also override the automatically
	 * generated Credential attribute.
	 */
	const u8 *extra_cred;

	/**
	 * extra_cred_len: Length of extra_cred in octets
	 */
	size_t extra_cred_len;

	/**
	 * disable_auto_conf - Disable auto-configuration on first registration
	 *
	 * By default, the AP that is started in not configured state will
	 * generate a random PSK and move to configured state when the first
	 * registration protocol run is completed successfully. This option can
	 * be used to disable this functionality and leave it up to an external
	 * program to take care of configuration. This requires the extra_cred
	 * to be set with a suitable Credential and skip_cred_build being used.
	 */
	int disable_auto_conf;

	/**
	 * static_wep_only - Whether the BSS supports only static WEP
	 */
	int static_wep_only;
};


/**
 * enum wps_event - WPS event types
 */
enum wps_event {
	/**
	 * WPS_EV_M2D - M2D received (Registrar did not know us)
	 */
	WPS_EV_M2D,

	/**
	 * WPS_EV_FAIL - Registration failed
	 */
	WPS_EV_FAIL,

	/**
	 * WPS_EV_SUCCESS - Registration succeeded
	 */
	WPS_EV_SUCCESS,

	/**
	 * WPS_EV_PWD_AUTH_FAIL - Password authentication failed
	 */
	WPS_EV_PWD_AUTH_FAIL,

	/**
	 * WPS_EV_PBC_OVERLAP - PBC session overlap detected
	 */
	WPS_EV_PBC_OVERLAP,

	/**
	 * WPS_EV_PBC_TIMEOUT - PBC walktime expired before protocol run start
	 */
	WPS_EV_PBC_TIMEOUT,

	/**
	 * WPS_EV_ER_AP_ADD - ER: AP added
	 */
	WPS_EV_ER_AP_ADD,

	/**
	 * WPS_EV_ER_AP_REMOVE - ER: AP removed
	 */
	WPS_EV_ER_AP_REMOVE,

	/**
	 * WPS_EV_ER_ENROLLEE_ADD - ER: Enrollee added
	 */
	WPS_EV_ER_ENROLLEE_ADD,

	/**
	 * WPS_EV_ER_ENROLLEE_REMOVE - ER: Enrollee removed
	 */
	WPS_EV_ER_ENROLLEE_REMOVE
};

/**
 * union wps_event_data - WPS event data
 */
union wps_event_data {
	/**
	 * struct wps_event_m2d - M2D event data
	 */
	struct wps_event_m2d {
		const u8 *manufacturer;
		size_t manufacturer_len;
		const u8 *model_name;
		size_t model_name_len;
		const u8 *model_number;
		size_t model_number_len;
		const u8 *serial_number;
		size_t serial_number_len;
		const u8 *dev_name;
		size_t dev_name_len;
		const u8 *primary_dev_type; /* 8 octets */
		u16 config_methods;
		u16 config_error;
		u16 dev_password_id;
	} m2d;

	/**
	 * struct wps_event_fail - Registration failure information
	 * @msg: enum wps_msg_type
	 */
	struct wps_event_fail {
		int msg;
	} fail;

	struct wps_event_pwd_auth_fail {
		int enrollee;
		int part;
	} pwd_auth_fail;

	struct wps_event_er_ap {
		const u8 *uuid;
		const u8 *mac_addr;
		const char *friendly_name;
		const char *manufacturer;
		const char *manufacturer_url;
		const char *model_description;
		const char *model_name;
		const char *model_number;
		const char *model_url;
		const char *serial_number;
		const char *upc;
		const u8 *pri_dev_type;
		u8 wps_state;
	} ap;

	struct wps_event_er_enrollee {
		const u8 *uuid;
		const u8 *mac_addr;
		int m1_received;
		u16 config_methods;
		u16 dev_passwd_id;
		const u8 *pri_dev_type;
		const char *dev_name;
		const char *manufacturer;
		const char *model_name;
		const char *model_number;
		const char *serial_number;
	} enrollee;
};

/**
 * struct upnp_pending_message - Pending PutWLANResponse messages
 * @next: Pointer to next pending message or %NULL
 * @addr: NewWLANEventMAC
 * @msg: NewMessage
 * @type: Message Type
 */
struct upnp_pending_message {
	struct upnp_pending_message *next;
	u8 addr[ETH_ALEN];
	struct wpabuf *msg;
	enum wps_msg_type type;
};

/**
 * struct wps_context - Long term WPS context data
 *
 * This data is stored at the higher layer Authenticator or Supplicant data
 * structures and it is maintained over multiple registration protocol runs.
 */
struct wps_context {
	/**
	 * ap - Whether the local end is an access point
	 */
	int ap;

	/**
	 * registrar - Pointer to WPS registrar data from wps_registrar_init()
	 */
	struct wps_registrar *registrar;

	/**
	 * wps_state - Current WPS state
	 */
	enum wps_state wps_state;

	/**
	 * ap_setup_locked - Whether AP setup is locked (only used at AP)
	 */
	int ap_setup_locked;

	/**
	 * uuid - Own UUID
	 */
	u8 uuid[16];

	/**
	 * ssid - SSID
	 *
	 * This SSID is used by the Registrar to fill in information for
	 * Credentials. In addition, AP uses it when acting as an Enrollee to
	 * notify Registrar of the current configuration.
	 */
	u8 ssid[32];

	/**
	 * ssid_len - Length of ssid in octets
	 */
	size_t ssid_len;

	/**
	 * dev - Own WPS device data
	 */
	struct wps_device_data dev;

	/**
	 * oob_conf - OOB Config data
	 */
	struct oob_conf_data oob_conf;

	/**
	 * oob_dev_pw_id - OOB Device password id
	 */
	u16 oob_dev_pw_id;

	/**
	 * dh_ctx - Context data for Diffie-Hellman operation
	 */
	void *dh_ctx;

	/**
	 * dh_privkey - Diffie-Hellman private key
	 */
	struct wpabuf *dh_privkey;

	/**
	 * dh_pubkey_oob - Diffie-Hellman public key
	 */
	struct wpabuf *dh_pubkey;

	/**
	 * config_methods - Enabled configuration methods
	 *
	 * Bit field of WPS_CONFIG_*
	 */
	u16 config_methods;

	/**
	 * encr_types - Enabled encryption types (bit field of WPS_ENCR_*)
	 */
	u16 encr_types;

	/**
	 * auth_types - Authentication types (bit field of WPS_AUTH_*)
	 */
	u16 auth_types;

	/**
	 * network_key - The current Network Key (PSK) or %NULL to generate new
	 *
	 * If %NULL, Registrar will generate per-device PSK. In addition, AP
	 * uses this when acting as an Enrollee to notify Registrar of the
	 * current configuration.
	 *
	 * When using WPA/WPA2-Person, this key can be either the ASCII
	 * passphrase (8..63 characters) or the 32-octet PSK (64 hex
	 * characters). When this is set to the ASCII passphrase, the PSK can
	 * be provided in the psk buffer and used per-Enrollee to control which
	 * key type is included in the Credential (e.g., to reduce calculation
	 * need on low-powered devices by provisioning PSK while still allowing
	 * other devices to get the passphrase).
	 */
	u8 *network_key;

	/**
	 * network_key_len - Length of network_key in octets
	 */
	size_t network_key_len;

	/**
	 * psk - The current network PSK
	 *
	 * This optional value can be used to provide the current PSK if
	 * network_key is set to the ASCII passphrase.
	 */
	u8 psk[32];

	/**
	 * psk_set - Whether psk value is set
	 */
	int psk_set;

	/**
	 * ap_settings - AP Settings override for M7 (only used at AP)
	 *
	 * If %NULL, AP Settings attributes will be generated based on the
	 * current network configuration.
	 */
	u8 *ap_settings;

	/**
	 * ap_settings_len - Length of ap_settings in octets
	 */
	size_t ap_settings_len;

	/**
	 * friendly_name - Friendly Name (required for UPnP)
	 */
	char *friendly_name;

	/**
	 * manufacturer_url - Manufacturer URL (optional for UPnP)
	 */
	char *manufacturer_url;

	/**
	 * model_description - Model Description (recommended for UPnP)
	 */
	char *model_description;

	/**
	 * model_url - Model URL (optional for UPnP)
	 */
	char *model_url;

	/**
	 * upc - Universal Product Code (optional for UPnP)
	 */
	char *upc;

	/**
	 * cred_cb - Callback to notify that new Credentials were received
	 * @ctx: Higher layer context data (cb_ctx)
	 * @cred: The received Credential
	 * Return: 0 on success, -1 on failure
	 */
	int (*cred_cb)(void *ctx, const struct wps_credential *cred);

	/**
	 * event_cb - Event callback (state information about progress)
	 * @ctx: Higher layer context data (cb_ctx)
	 * @event: Event type
	 * @data: Event data
	 */
	void (*event_cb)(void *ctx, enum wps_event event,
			 union wps_event_data *data);

	/**
	 * cb_ctx: Higher layer context data for callbacks
	 */
	void *cb_ctx;

	struct upnp_wps_device_sm *wps_upnp;

	/* Pending messages from UPnP PutWLANResponse */
	struct upnp_pending_message *upnp_msgs;
};

struct oob_device_data {
	char *device_name;
	char *device_path;
	void * (*init_func)(struct wps_context *, struct oob_device_data *,
			    int);
	struct wpabuf * (*read_func)(void *);
	int (*write_func)(void *, struct wpabuf *);
	void (*deinit_func)(void *);
};

struct oob_nfc_device_data {
	int (*init_func)(char *);
	void * (*read_func)(size_t *);
	int (*write_func)(void *, size_t);
	void (*deinit_func)(void);
};

struct wps_registrar *
wps_registrar_init(struct wps_context *wps,
		   const struct wps_registrar_config *cfg);
void wps_registrar_deinit(struct wps_registrar *reg);
int wps_registrar_add_pin(struct wps_registrar *reg, const u8 *uuid,
			  const u8 *pin, size_t pin_len, int timeout);
int wps_registrar_invalidate_pin(struct wps_registrar *reg, const u8 *uuid);
int wps_registrar_unlock_pin(struct wps_registrar *reg, const u8 *uuid);
int wps_registrar_button_pushed(struct wps_registrar *reg);
void wps_registrar_probe_req_rx(struct wps_registrar *reg, const u8 *addr,
				const struct wpabuf *wps_data);
int wps_registrar_update_ie(struct wps_registrar *reg);
int wps_registrar_get_info(struct wps_registrar *reg, const u8 *addr,
			   char *buf, size_t buflen);

unsigned int wps_pin_checksum(unsigned int pin);
unsigned int wps_pin_valid(unsigned int pin);
unsigned int wps_generate_pin(void);
void wps_free_pending_msgs(struct upnp_pending_message *msgs);

struct oob_device_data * wps_get_oob_device(char *device_type);
struct oob_nfc_device_data * wps_get_oob_nfc_device(char *device_name);
int wps_get_oob_method(char *method);
int wps_process_oob(struct wps_context *wps, struct oob_device_data *oob_dev,
		    int registrar);
int wps_attr_text(struct wpabuf *data, char *buf, char *end);

struct wps_er * wps_er_init(struct wps_context *wps, const char *ifname);
void wps_er_refresh(struct wps_er *er);
void wps_er_deinit(struct wps_er *er, void (*cb)(void *ctx), void *ctx);
void wps_er_set_sel_reg(struct wps_er *er, int sel_reg, u16 dev_passwd_id,
			u16 sel_reg_config_methods);
int wps_er_pbc(struct wps_er *er, const u8 *uuid);
int wps_er_learn(struct wps_er *er, const u8 *uuid, const u8 *pin,
		 size_t pin_len);

int wps_dev_type_str2bin(const char *str, u8 dev_type[WPS_DEV_TYPE_LEN]);
char * wps_dev_type_bin2str(const u8 dev_type[WPS_DEV_TYPE_LEN], char *buf,
			    size_t buf_len);
void uuid_gen_mac_addr(const u8 *mac_addr, u8 *uuid);
u16 wps_config_methods_str2bin(const char *str);

#endif /* WPS_H */
