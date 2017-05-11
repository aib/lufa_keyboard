#include <LUFA/Drivers/USB/USB.h>

#include <avr/io.h>

static uint8_t report_buffer_prev[sizeof(USB_KeyboardReport_Data_t)];

bool button_pressed = false;
bool button_last_up = true;

USB_ClassInfo_HID_Device_t hid_info = {
	.Config = {
		.InterfaceNumber = 0,
		.ReportINEndpoint = {
			.Address = ENDPOINT_DIR_IN | 1,
			.Size = 8,
			.Banks = 1,
		},
		.PrevReportINBuffer = report_buffer_prev,
		.PrevReportINBufferSize = sizeof(report_buffer_prev),
	},
};

void button_tasks(void)
{
	bool button_up = PINC & 64;

	if (button_last_up && !button_up) {
		button_pressed = true;
	}

	button_last_up = button_up;
}

int main(void)
{
	USB_Init();

	DDRB = 1;
	DDRC = 0;
	DDRD = 32;

	PORTB = 1;
	PORTC = 64; //Enable pull-up on PC6
	PORTD = 32;

	GlobalInterruptEnable();

	while (true) {
		HID_Device_USBTask(&hid_info);
		USB_USBTask();
		button_tasks();
	}
}

void EVENT_USB_Device_Connect(void)
{
	PORTD &= ~32;
}

void EVENT_USB_Device_Disconnect(void)
{
	PORTD |= 32;
}

void EVENT_USB_Device_ConfigurationChanged(void)
{
	HID_Device_ConfigureEndpoints(&hid_info);
	USB_Device_EnableSOFEvents();
}

void EVENT_USB_Device_ControlRequest(void)
{
	HID_Device_ProcessControlRequest(&hid_info);
}

void EVENT_USB_Device_StartOfFrame(void)
{
	HID_Device_MillisecondElapsed(&hid_info);
}

bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo, uint8_t* const ReportID, const uint8_t ReportType, void* ReportData, uint16_t* const ReportSize)
{
	USB_KeyboardReport_Data_t *report = (USB_KeyboardReport_Data_t*) ReportData;

	if (button_pressed) {
		report->KeyCode[0] = HID_KEYBOARD_SC_CAPS_LOCK;
		button_pressed = false;
	}

	*ReportSize = sizeof(USB_KeyboardReport_Data_t);
	return false;
}

void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo, const uint8_t ReportID, const uint8_t ReportType, const void* ReportData, const uint16_t ReportSize)
{
	uint8_t leds = *(uint8_t*) ReportData;

	if (leds & HID_KEYBOARD_LED_CAPSLOCK) {
		PORTB &= ~1;
	} else {
		PORTB |= 1;
	}
}
