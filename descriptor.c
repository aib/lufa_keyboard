#include <avr/pgmspace.h>
#include <LUFA/Drivers/USB/USB.h>

struct usb_descriptor {
	USB_Descriptor_Configuration_Header_t config;

	USB_Descriptor_Interface_t hid_interface;
	USB_HID_Descriptor_HID_t   hid;
	USB_Descriptor_Endpoint_t  endpoint;
};

enum string_ids_t {
	STRID_LANGUAGE,
	STRID_MANUFACTURER,
	STRID_PRODUCT
};

const USB_Descriptor_String_t PROGMEM str_language = USB_STRING_DESCRIPTOR_ARRAY(LANGUAGE_ID_ENG);
const USB_Descriptor_String_t PROGMEM str_manufacturer = USB_STRING_DESCRIPTOR(L"aib");
const USB_Descriptor_String_t PROGMEM str_product = USB_STRING_DESCRIPTOR(L"LUFA Keyboard");

const USB_Descriptor_HIDReport_Datatype_t PROGMEM desc_hid_report[] = {
	HID_DESCRIPTOR_KEYBOARD(6)
};

const USB_Descriptor_Device_t PROGMEM desc_device = {
	.Header                 = { .Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device },

	.USBSpecification       = VERSION_BCD(1, 1, 0),
	.Class                  = USB_CSCP_NoDeviceClass,
	.SubClass               = USB_CSCP_NoDeviceSubclass,
	.Protocol               = USB_CSCP_NoDeviceProtocol,

	.Endpoint0Size          = FIXED_CONTROL_ENDPOINT_SIZE,

	.VendorID               = 0x03eb, //Atmel
	.ProductID              = 0x2042, //LUFA Keyboard Demo
	.ReleaseNumber          = VERSION_BCD(1, 0, 0),

	.ManufacturerStrIndex   = STRID_MANUFACTURER,
	.ProductStrIndex        = STRID_PRODUCT,
	.SerialNumStrIndex      = NO_DESCRIPTOR,

	.NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

const struct usb_descriptor PROGMEM desc_config = {
	.config = {
		.Header                 = { .Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration },
		.TotalConfigurationSize = sizeof(struct usb_descriptor),
		.TotalInterfaces        = 1,
		.ConfigurationNumber    = 1,
		.ConfigurationStrIndex  = NO_DESCRIPTOR,
		.ConfigAttributes       = (USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED),
		.MaxPowerConsumption    = USB_CONFIG_POWER_MA(100)
	},

	.hid_interface = {
		.Header            = { .Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface },
		.InterfaceNumber   = 0,
		.AlternateSetting  = 0x00,
		.TotalEndpoints    = 1,
		.Class             = HID_CSCP_HIDClass,
		.SubClass          = HID_CSCP_BootSubclass,
		.Protocol          = HID_CSCP_KeyboardBootProtocol,
		.InterfaceStrIndex = NO_DESCRIPTOR
	},

	.hid = {
		.Header                 = { .Size = sizeof(USB_HID_Descriptor_HID_t), .Type = HID_DTYPE_HID },
		.HIDSpec                = VERSION_BCD(1, 1, 1),
		.CountryCode            = 0x00,
		.TotalReportDescriptors = 1,
		.HIDReportType          = HID_DTYPE_Report,
		.HIDReportLength        = sizeof(desc_hid_report)
	},

	.endpoint = {
		.Header            = { .Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint },
		.EndpointAddress   = ENDPOINT_DIR_IN | 1,
		.Attributes        = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
		.EndpointSize      = 8,
		.PollingIntervalMS = 0x05
	},
};

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue, const uint16_t wIndex, const void** const DescriptorAddress)
{
	const uint8_t desc_type = (wValue >> 8);
	const uint8_t desc_num = (wValue & 0xFF);

	switch (desc_type) {
		case DTYPE_Device:
			*DescriptorAddress = &desc_device;
			return desc_device.Header.Size;

		case DTYPE_Configuration:
			*DescriptorAddress = &desc_config;
			return desc_config.config.TotalConfigurationSize;

		case DTYPE_String:
			switch (desc_num) {
				case STRID_LANGUAGE:
					*DescriptorAddress = &str_language;
					return str_language.Header.Size;

				case STRID_MANUFACTURER:
					*DescriptorAddress = &str_manufacturer;
					return str_manufacturer.Header.Size;

				case STRID_PRODUCT:
					*DescriptorAddress = &str_product;
					return str_product.Header.Size;
			}

		case HID_DTYPE_HID:
			*DescriptorAddress = &desc_config.hid;
			return desc_config.hid.Header.Size;

		case HID_DTYPE_Report:
			*DescriptorAddress = &desc_hid_report;
			return sizeof(desc_hid_report);
	}

	return NO_DESCRIPTOR;
}
