//==============================================================================
// Video.cpp
// Windows support YUY2 and NV12
// https://docs.microsoft.com/en-us/windows-hardware/drivers/stream/usb-video-class-driver-overview
//==============================================================================
#include "jxglib/USBDevice/Video.h"

struct TU_ATTR_PACKED uvc_control_desc_t {
	tusb_desc_interface_t itf;
	tusb_desc_video_control_header_1itf_t header;
	tusb_desc_video_control_camera_terminal_t camera_terminal;
	tusb_desc_video_control_output_terminal_t output_terminal;
};

struct TU_ATTR_PACKED uvc_streaming_desc_t {
	tusb_desc_interface_t itf;
	tusb_desc_video_streaming_input_header_1byte_t header;
	tusb_desc_video_format_uncompressed_t format;
	tusb_desc_video_frame_uncompressed_continuous_t frame;
	tusb_desc_video_streaming_color_matching_t color;
#if !CFG_TUD_VIDEO_STREAMING_BULK
	// For ISO streaming, USB spec requires to alternate interface
	tusb_desc_interface_t itf_alt;
#endif
	tusb_desc_endpoint_t ep;
};
struct TU_ATTR_PACKED ConfigDesc {
	tusb_desc_interface_assoc_t iad;
	uvc_control_desc_t video_control;
	uvc_streaming_desc_t video_streaming;
};

#if CFG_TUD_VIDEO > 0

//-----------------------------------------------------------------------------
// USBDevice::Video
//-----------------------------------------------------------------------------
namespace jxglib::USBDevice {

Video::Video(Controller& deviceController, const char* strControl, const char* strStreaming, uint8_t endp, int width, int height, int frameRate) : Interface(deviceController, 2)
{
	uint32_t clockFrequency = 27000000; // Time stamp base clock. It is a deprecated parameter.
	uint8_t UVC_ENTITY_CAP_INPUT_TERMINAL = 0x01;
	uint8_t UVC_ENTITY_CAP_OUTPUT_TERMINAL = 0x02;
	uint8_t interfaceNum_VIDEO_CONTROL = interfaceNum_;
	uint8_t interfaceNum_VIDEO_STREAMING = interfaceNum_ + 1;
	const ConfigDesc configDesc = {
		.iad = {
			.bLength = sizeof(tusb_desc_interface_assoc_t),
			.bDescriptorType = TUSB_DESC_INTERFACE_ASSOCIATION,

			.bFirstInterface = interfaceNum_VIDEO_CONTROL,
			.bInterfaceCount = 2,
			.bFunctionClass = TUSB_CLASS_VIDEO,
			.bFunctionSubClass = VIDEO_SUBCLASS_INTERFACE_COLLECTION,
			.bFunctionProtocol = VIDEO_ITF_PROTOCOL_UNDEFINED,
			.iFunction = 0
		},
		.video_control = {
			.itf = {
				.bLength = sizeof(tusb_desc_interface_t),
				.bDescriptorType = TUSB_DESC_INTERFACE,

				.bInterfaceNumber = interfaceNum_VIDEO_CONTROL,
				.bAlternateSetting = 0,
				.bNumEndpoints = 0,
				.bInterfaceClass = TUSB_CLASS_VIDEO,
				.bInterfaceSubClass = VIDEO_SUBCLASS_CONTROL,
				.bInterfaceProtocol = VIDEO_ITF_PROTOCOL_15,
				.iInterface = deviceController.RegisterStringDesc(strControl)
			},
			.header = {
				.bLength = sizeof(tusb_desc_video_control_header_1itf_t),
				.bDescriptorType = TUSB_DESC_CS_INTERFACE,
				.bDescriptorSubType = VIDEO_CS_ITF_VC_HEADER,

				.bcdUVC = VIDEO_BCD_1_50,
				.wTotalLength = sizeof(uvc_control_desc_t) - sizeof(tusb_desc_interface_t), // CS VC descriptors only
				.dwClockFrequency = clockFrequency,
				.bInCollection = 1,
				.baInterfaceNr = { interfaceNum_VIDEO_STREAMING }
			},
			.camera_terminal = {
				.bLength = sizeof(tusb_desc_video_control_camera_terminal_t),
				.bDescriptorType = TUSB_DESC_CS_INTERFACE,
				.bDescriptorSubType = VIDEO_CS_ITF_VC_INPUT_TERMINAL,

				.bTerminalID = UVC_ENTITY_CAP_INPUT_TERMINAL,
				.wTerminalType = VIDEO_ITT_CAMERA,
				.bAssocTerminal = 0,
				.iTerminal = 0,
				.wObjectiveFocalLengthMin = 0,
				.wObjectiveFocalLengthMax = 0,
				.wOcularFocalLength = 0,
				.bControlSize = 3,
				.bmControls = { 0, 0, 0 }
			},
			.output_terminal = {
				.bLength = sizeof(tusb_desc_video_control_output_terminal_t),
				.bDescriptorType = TUSB_DESC_CS_INTERFACE,
				.bDescriptorSubType = VIDEO_CS_ITF_VC_OUTPUT_TERMINAL,

				.bTerminalID = UVC_ENTITY_CAP_OUTPUT_TERMINAL,
				.wTerminalType = VIDEO_TT_STREAMING,
				.bAssocTerminal = 0,
				.bSourceID = UVC_ENTITY_CAP_INPUT_TERMINAL,
				.iTerminal = 0
			}
		},
		.video_streaming = {
			.itf = {
				.bLength = sizeof(tusb_desc_interface_t),
				.bDescriptorType = TUSB_DESC_INTERFACE,

				.bInterfaceNumber = interfaceNum_VIDEO_STREAMING,
				.bAlternateSetting = 0,
				.bNumEndpoints = CFG_TUD_VIDEO_STREAMING_BULK, // bulk 1, iso 0
				.bInterfaceClass = TUSB_CLASS_VIDEO,
				.bInterfaceSubClass = VIDEO_SUBCLASS_STREAMING,
				.bInterfaceProtocol = VIDEO_ITF_PROTOCOL_15,
				.iInterface = deviceController.RegisterStringDesc(strStreaming)
			},
			.header = {
				.bLength = sizeof(tusb_desc_video_streaming_input_header_1byte_t),
				.bDescriptorType = TUSB_DESC_CS_INTERFACE,
				.bDescriptorSubType = VIDEO_CS_ITF_VS_INPUT_HEADER,

				.bNumFormats = 1,
				.wTotalLength = sizeof(uvc_streaming_desc_t) - sizeof(tusb_desc_interface_t)
					- sizeof(tusb_desc_endpoint_t) - (CFG_TUD_VIDEO_STREAMING_BULK? 0 : sizeof(tusb_desc_interface_t)), // CS VS descriptors only
				.bEndpointAddress = endp,
				.bmInfo = 0,
				.bTerminalLink = UVC_ENTITY_CAP_OUTPUT_TERMINAL,
				.bStillCaptureMethod = 0,
				.bTriggerSupport = 0,
				.bTriggerUsage = 0,
				.bControlSize = 1,
				.bmaControls = { 0 }
			},
			.format = {
				.bLength = sizeof(tusb_desc_video_format_uncompressed_t),
				.bDescriptorType = TUSB_DESC_CS_INTERFACE,
				.bDescriptorSubType = VIDEO_CS_ITF_VS_FORMAT_UNCOMPRESSED,
				.bFormatIndex = 1, // 1-based index
				.bNumFrameDescriptors = 1,
				.guidFormat = { TUD_VIDEO_GUID_YUY2 },
				.bBitsPerPixel = 16,
				.bDefaultFrameIndex = 1,
				.bAspectRatioX = 0,
				.bAspectRatioY = 0,
				.bmInterlaceFlags = 0,
				.bCopyProtect = 0
			},
			.frame = {
				.bLength = sizeof(tusb_desc_video_frame_uncompressed_continuous_t),
				.bDescriptorType = TUSB_DESC_CS_INTERFACE,
				.bDescriptorSubType = VIDEO_CS_ITF_VS_FRAME_UNCOMPRESSED,
				.bFrameIndex = 1, // 1-based index
				.bmCapabilities = 0,
				.wWidth = static_cast<uint16_t>(width),
				.wHeight = static_cast<uint16_t>(height),
				.dwMinBitRate = static_cast<uint32_t>(width * height * 16 * 1),
				.dwMaxBitRate = static_cast<uint32_t>(width * height * 16 * frameRate),
				.dwMaxVideoFrameBufferSize = static_cast<uint32_t>(width * height * 16 / 8),
				.dwDefaultFrameInterval = static_cast<uint32_t>(10000000 / frameRate),
				.bFrameIntervalType = 0, // continuous
				.dwFrameInterval = {
					static_cast<uint32_t>(10000000 / frameRate), // min
					static_cast<uint32_t>(10000000), // max
					static_cast<uint32_t>(10000000 / frameRate) // step
				}
			},
			.color = {
				.bLength = sizeof(tusb_desc_video_streaming_color_matching_t),
				.bDescriptorType = TUSB_DESC_CS_INTERFACE,
				.bDescriptorSubType = VIDEO_CS_ITF_VS_COLORFORMAT,

				.bColorPrimaries = VIDEO_COLOR_PRIMARIES_BT709,
				.bTransferCharacteristics = VIDEO_COLOR_XFER_CH_BT709,
				.bMatrixCoefficients = VIDEO_COLOR_COEF_SMPTE170M
			},
#if !CFG_TUD_VIDEO_STREAMING_BULK
			.itf_alt = {
				.bLength = sizeof(tusb_desc_interface_t),
				.bDescriptorType = TUSB_DESC_INTERFACE,

				.bInterfaceNumber = interfaceNum_VIDEO_STREAMING,
				.bAlternateSetting = 1,
				.bNumEndpoints = 1,
				.bInterfaceClass = TUSB_CLASS_VIDEO,
				.bInterfaceSubClass = VIDEO_SUBCLASS_STREAMING,
				.bInterfaceProtocol = VIDEO_ITF_PROTOCOL_15,
				.iInterface = deviceController.RegisterStringDesc(strStreaming)
			},
#endif
			.ep = {
				.bLength = sizeof(tusb_desc_endpoint_t),
				.bDescriptorType = TUSB_DESC_ENDPOINT,

				.bEndpointAddress = endp,
				.bmAttributes = {
					.xfer = CFG_TUD_VIDEO_STREAMING_BULK? TUSB_XFER_BULK : TUSB_XFER_ISOCHRONOUS,
					.sync = CFG_TUD_VIDEO_STREAMING_BULK? 0 : 1 // asynchronous
				},
				.wMaxPacketSize = CFG_TUD_VIDEO_STREAMING_BULK? 64 : CFG_TUD_VIDEO_STREAMING_EP_BUFSIZE,
				.bInterval = 1
			}
		}
	};
	RegisterConfigDesc(&configDesc, sizeof(configDesc));
}

}

//-----------------------------------------------------------------------------
// Callback functions
//-----------------------------------------------------------------------------

#endif
