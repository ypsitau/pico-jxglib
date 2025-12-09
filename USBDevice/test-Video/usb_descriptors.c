#include "tusb.h"

#define FRAME_WIDTH   128
#define FRAME_HEIGHT  96
#define FRAME_RATE    10

/* Windows support YUY2 and NV12
 * https://docs.microsoft.com/en-us/windows-hardware/drivers/stream/usb-video-class-driver-overview */

// String Descriptor Index
enum {
	STRID_LANGID = 0,
	STRID_MANUFACTURER,
	STRID_PRODUCT,
	STRID_SERIAL,
	STRID_UVC_CONTROL = 0,
	STRID_UVC_STREAMING = 0,
};

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+

/* Time stamp base clock. It is a deprecated parameter. */
#define UVC_CLOCK_FREQUENCY    27000000

/* video capture path */
#define UVC_ENTITY_CAP_INPUT_TERMINAL  0x01
#define UVC_ENTITY_CAP_OUTPUT_TERMINAL 0x02

enum {
	ITF_NUM_VIDEO_CONTROL,
	ITF_NUM_VIDEO_STREAMING,
	ITF_NUM_TOTAL
};

#define EPNUM_VIDEO_IN    0x81

#if defined(CFG_EXAMPLE_VIDEO_READONLY) && !defined(CFG_EXAMPLE_VIDEO_DISABLE_MJPEG)
	#define USE_MJPEG 1
#else
	#define USE_MJPEG 0
#endif

#define USE_ISO_STREAMING (!CFG_TUD_VIDEO_STREAMING_BULK)

typedef struct TU_ATTR_PACKED {
	tusb_desc_interface_t itf;
	tusb_desc_video_control_header_1itf_t header;
	tusb_desc_video_control_camera_terminal_t camera_terminal;
	tusb_desc_video_control_output_terminal_t output_terminal;
} uvc_control_desc_t;

/* Windows support YUY2 and NV12
 * https://docs.microsoft.com/en-us/windows-hardware/drivers/stream/usb-video-class-driver-overview */

typedef struct TU_ATTR_PACKED {
	tusb_desc_interface_t itf;
	tusb_desc_video_streaming_input_header_1byte_t header;
#if USE_MJPEG
	tusb_desc_video_format_mjpeg_t format;
	tusb_desc_video_frame_mjpeg_continuous_t frame;
#else
	tusb_desc_video_format_uncompressed_t format;
	tusb_desc_video_frame_uncompressed_continuous_t frame;
#endif
	tusb_desc_video_streaming_color_matching_t color;
#if USE_ISO_STREAMING
	// For ISO streaming, USB spec requires to alternate interface
	tusb_desc_interface_t itf_alt;
#endif
	tusb_desc_endpoint_t ep;
} uvc_streaming_desc_t;

typedef struct TU_ATTR_PACKED {
	tusb_desc_interface_assoc_t iad;
	uvc_control_desc_t video_control;
	uvc_streaming_desc_t video_streaming;
} uvc_cfg_desc_t;

const uvc_cfg_desc_t desc_fs_configuration = {
	.iad = {
		.bLength = sizeof(tusb_desc_interface_assoc_t),
		.bDescriptorType = TUSB_DESC_INTERFACE_ASSOCIATION,

		.bFirstInterface = ITF_NUM_VIDEO_CONTROL,
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

			.bInterfaceNumber = ITF_NUM_VIDEO_CONTROL,
			.bAlternateSetting = 0,
			.bNumEndpoints = 0,
			.bInterfaceClass = TUSB_CLASS_VIDEO,
			.bInterfaceSubClass = VIDEO_SUBCLASS_CONTROL,
			.bInterfaceProtocol = VIDEO_ITF_PROTOCOL_15,
			.iInterface = STRID_UVC_CONTROL
		},
		.header = {
			.bLength = sizeof(tusb_desc_video_control_header_1itf_t),
			.bDescriptorType = TUSB_DESC_CS_INTERFACE,
			.bDescriptorSubType = VIDEO_CS_ITF_VC_HEADER,

			.bcdUVC = VIDEO_BCD_1_50,
			.wTotalLength = sizeof(uvc_control_desc_t) - sizeof(tusb_desc_interface_t), // CS VC descriptors only
			.dwClockFrequency = UVC_CLOCK_FREQUENCY,
			.bInCollection = 1,
			.baInterfaceNr = { ITF_NUM_VIDEO_STREAMING }
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

			.bInterfaceNumber = ITF_NUM_VIDEO_STREAMING,
			.bAlternateSetting = 0,
			.bNumEndpoints = CFG_TUD_VIDEO_STREAMING_BULK, // bulk 1, iso 0
			.bInterfaceClass = TUSB_CLASS_VIDEO,
			.bInterfaceSubClass = VIDEO_SUBCLASS_STREAMING,
			.bInterfaceProtocol = VIDEO_ITF_PROTOCOL_15,
			.iInterface = STRID_UVC_STREAMING
		},
		.header = {
			.bLength = sizeof(tusb_desc_video_streaming_input_header_1byte_t),
			.bDescriptorType = TUSB_DESC_CS_INTERFACE,
			.bDescriptorSubType = VIDEO_CS_ITF_VS_INPUT_HEADER,

			.bNumFormats = 1,
			.wTotalLength = sizeof(uvc_streaming_desc_t) - sizeof(tusb_desc_interface_t)
				- sizeof(tusb_desc_endpoint_t) - (USE_ISO_STREAMING ? sizeof(tusb_desc_interface_t) : 0) , // CS VS descriptors only
			.bEndpointAddress = EPNUM_VIDEO_IN,
			.bmInfo = 0,
			.bTerminalLink = UVC_ENTITY_CAP_OUTPUT_TERMINAL,
			.bStillCaptureMethod = 0,
			.bTriggerSupport = 0,
			.bTriggerUsage = 0,
			.bControlSize = 1,
			.bmaControls = { 0 }
		},
		.format = {
#if USE_MJPEG
			.bLength = sizeof(tusb_desc_video_format_mjpeg_t),
			.bDescriptorType = TUSB_DESC_CS_INTERFACE,
			.bDescriptorSubType = VIDEO_CS_ITF_VS_FORMAT_MJPEG,
			.bFormatIndex = 1, // 1-based index
			.bNumFrameDescriptors = 1,
			.bmFlags = 0,
#else
			.bLength = sizeof(tusb_desc_video_format_uncompressed_t),
			.bDescriptorType = TUSB_DESC_CS_INTERFACE,
			.bDescriptorSubType = VIDEO_CS_ITF_VS_FORMAT_UNCOMPRESSED,
			.bFormatIndex = 1, // 1-based index
			.bNumFrameDescriptors = 1,
			.guidFormat = { TUD_VIDEO_GUID_YUY2 },
			.bBitsPerPixel = 16,
#endif
			.bDefaultFrameIndex = 1,
			.bAspectRatioX = 0,
			.bAspectRatioY = 0,
			.bmInterlaceFlags = 0,
			.bCopyProtect = 0
		},
		.frame = {
#if USE_MJPEG
			.bLength = sizeof(tusb_desc_video_frame_mjpeg_continuous_t),
			.bDescriptorType = TUSB_DESC_CS_INTERFACE,
			.bDescriptorSubType = VIDEO_CS_ITF_VS_FRAME_MJPEG,
#else
			.bLength = sizeof(tusb_desc_video_frame_uncompressed_continuous_t),
			.bDescriptorType = TUSB_DESC_CS_INTERFACE,
			.bDescriptorSubType = VIDEO_CS_ITF_VS_FRAME_UNCOMPRESSED,
#endif
			.bFrameIndex = 1, // 1-based index
			.bmCapabilities = 0,
			.wWidth = FRAME_WIDTH,
			.wHeight = FRAME_HEIGHT,
			.dwMinBitRate = FRAME_WIDTH * FRAME_HEIGHT * 16 * 1,
			.dwMaxBitRate = FRAME_WIDTH * FRAME_HEIGHT * 16 * FRAME_RATE,
			.dwMaxVideoFrameBufferSize = FRAME_WIDTH * FRAME_HEIGHT * 16 / 8,
			.dwDefaultFrameInterval = 10000000 / FRAME_RATE,
			.bFrameIntervalType = 0, // continuous
			.dwFrameInterval = {
				10000000 / FRAME_RATE, // min
				10000000, // max
				10000000 / FRAME_RATE // step
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
#if USE_ISO_STREAMING
		.itf_alt = {
			.bLength = sizeof(tusb_desc_interface_t),
			.bDescriptorType = TUSB_DESC_INTERFACE,

			.bInterfaceNumber = ITF_NUM_VIDEO_STREAMING,
			.bAlternateSetting = 1,
			.bNumEndpoints = 1,
			.bInterfaceClass = TUSB_CLASS_VIDEO,
			.bInterfaceSubClass = VIDEO_SUBCLASS_STREAMING,
			.bInterfaceProtocol = VIDEO_ITF_PROTOCOL_15,
			.iInterface = STRID_UVC_STREAMING
		},
#endif
		.ep = {
			.bLength = sizeof(tusb_desc_endpoint_t),
			.bDescriptorType = TUSB_DESC_ENDPOINT,

			.bEndpointAddress = EPNUM_VIDEO_IN,
			.bmAttributes = {
				.xfer = CFG_TUD_VIDEO_STREAMING_BULK ? TUSB_XFER_BULK : TUSB_XFER_ISOCHRONOUS,
				.sync = CFG_TUD_VIDEO_STREAMING_BULK ? 0 : 1 // asynchronous
			},
			.wMaxPacketSize = CFG_TUD_VIDEO_STREAMING_BULK ? 64 : CFG_TUD_VIDEO_STREAMING_EP_BUFSIZE,
			.bInterval = 1
		}
	}
};

const uint8_t* GetConfigDesc(int* pBytes)
{
	*pBytes = sizeof(desc_fs_configuration);
	return (const uint8_t*)&desc_fs_configuration;
}
