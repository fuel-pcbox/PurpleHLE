#include "IoServicePort.h"
#include "TaskPort.h"
#include "FrameBufferPort.h"
#include "MbxPort.h"
#include "MiscIo.h"
#include "StringUtils.h"

static std::string Provider(const std::string& provider)
{
	return _S("<key>IOProviderClass</key><string>") + provider + _S("</string>");
}
static std::string TrueProp(const std::string& prop)
{
	return _S("<key>") + prop + ("</key><true/>");
}
static std::string StrProp(const std::string& prop, const std::string& val)
{
	return _S("<key>") + prop + ("</key><string>" + val + "</string>");
}

std::string hex2data(std::string hex)
{
	std::string data;
	char base64[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
					  "abcdefghijklmnopqrstuvwxyz"
					  "0123456789+/";

	for (int i = 0; i < hex.size(); i += 6)
	{
		std::string bs1 = hex.substr(i, 2);
		std::string bs2 = (i + 2) < hex.size() ? hex.substr(i + 2, 2) : "";
		std::string bs3 = (i + 4) < hex.size() ? hex.substr(i + 4, 2) : "";

		u8 b1 = strtol(bs1.c_str(), NULL, 16);
		u8 b2 = strtol(bs2.c_str(), NULL, 16);
		u8 b3 = strtol(bs3.c_str(), NULL, 16);

		data += base64[ (b1 >> 2) & 0x3f ];
		data += base64[ ((b1 << 4) + (b2 >> 4)) & 0x3f ];
		data += bs2.empty() ? '=' : base64[ ((b2 << 2) + (b3 >> 6)) & 0x3f ];
		data += bs3.empty() ? '=' : base64[ b3 & 0x3f ];		
	}

	return data;
}

IoServicePort* IoServicePort::BuildAll()
{
	//EVERYTHING IS TOTALLY INCORRECT

	IoServicePort* root = new IoServicePort();

	IoServicePort *powerTree, *deviceTree, *resources;
	root->AddSubPort("IOPower:", powerTree = new IoServicePort());
	root->AddSubPort("IODeviceTree:", deviceTree = new IoServicePort());
	root->AddSubPort("IOResources:", resources = new IoServicePort()); //temp name

	IoServicePort *armIo;
	deviceTree->AddSubPort("arm-io", armIo = new IoServicePort(
		StrProp("chip-revision", "aaaaa")));
	//	"<key>chip-revision</key><data>" + hex2data("14000000") + "</data>"));

	IoServicePort *powerConn, *powerRoot;
	powerTree->AddSubPort("IOPowerConnection", powerConn = new IoServicePort());
	powerConn->AddSubPort("IOPMrootDomain", powerRoot = new IoServicePort("", new PMRootPort()));

	IoServicePort *mbx, *mbxCon, *mbxDev;
	armIo->AddSubPort("mbx", mbx = new IoServicePort());
	mbx->AddSubPort("AppleMBXController", mbxCon = new IoServicePort()); 
	mbxCon->AddSubPort("AppleMBXDevice", mbxDev = new IoServicePort(
		Provider("AppleMBXDevice"), new MbxPort()));

	IoServicePort *clcd, *clcdH1, *clcdM2, *ipuMX31;
	armIo->AddSubPort("clcd", clcd = new IoServicePort());
	clcd->AddSubPort("AppleH1CLCD", clcdH1 = new IoServicePort(
		Provider("AppleH1CLCD"), new LcdPort()));
	clcd->AddSubPort("AppleM2CLCD", clcdM2 = new IoServicePort(
		Provider("AppleM2CLCD"), new LcdPort()));
	clcd->AddSubPort("AppleMX31IPU", ipuMX31 = new IoServicePort(
		Provider("AppleMX31IPU"), new IpuPort())); // what is this?!
	
	IoServicePort *tvOut, *tvOutH1, *tvOutM2;
	armIo->AddSubPort("tvOut", tvOut = new IoServicePort());
	tvOut->AddSubPort("AppleH1TVOut", tvOutH1 = new IoServicePort(
		Provider("AppleH1TVOut"), new TvOutPort()));
	tvOut->AddSubPort("AppleM2TVOut", tvOutM2 = new IoServicePort(
		Provider("AppleM2TVOut"), new TvOutPort()));

	IoServicePort *aes, *aesCon;
	armIo->AddSubPort("aes", aes = new IoServicePort());
	aes->AddSubPort("AppleS5L8900XAES", aesCon = new IoServicePort(
		Provider("IOAESAccelerator"), new AesPort()));

	IoServicePort *surfaceRoot;
	resources->AddSubPort("IOCoreSurfaceRoot", surfaceRoot = new IoServicePort(
		Provider("IOCoreSurfaceRoot"), new SurfaceRootPort()));
	
	IoServicePort *unk, *powSrc, *mtTouch, *backLight;
	root->AddSubPort("UNKUNKUNK", unk = new IoServicePort());
	unk->AddSubPort("UNKpowSrc", powSrc = new IoServicePort(
		Provider("IOPMPowerSource"), new PowerSrcPort()));
	unk->AddSubPort("UNKmtTouch", mtTouch = new IoServicePort(
		Provider("IOService") + TrueProp("HIDServiceSupport") + 
		"<key>IOCFPlugInTypes</key><dict><key>0516B563-B15B-11DA-96EB-0014519758EF</key>"
			"<string>AppleMultitouchSPI.kext/PlugIns/MultitouchHID.plugin</string></dict>"
		"<key>IOClass</key><string>AppleMultitouchN1SPI</string>"
		"<key>Multitouch ID</key><integer>11596000</integer>"
		"<key>Family ID</key><integer>84</integer>"
		"<key>bcdVersion</key><integer>102</integer>"
		//"<key>Buttons</key>
		"<key>Sensor Rows</key><integer>16</integer>"
		"<key>Sensor Columns</key><integer>10</integer>"
		"<key>Endianness</key><integer>1</integer>"
		"<key>Sensor Surface Width</key><integer>5000</integer>"
		"<key>Sensor Surface Height</key><integer>7500</integer>"
		"<key>Multitouch Serial Number</key><string>none</string>"
		"<key>Sensor Region Descriptor</key><data>" + hex2data("0201000f01000a00030f010100010a") + "</data>"
		"<key>Sensor Region Param</key><data>" + hex2data("00000c000003") + "</data>"
		"<key>IONameMatched</key><string>multi-touch,n88</string>"
		, new MultiTouchPort()));
	unk->AddSubPort("UNKbackLight", backLight = new IoServicePort(
		TrueProp("backlight-control") + 
		"<key>IODisplayParameters</key><dict><key>brightness</key><dict>"
				"<key>min</key><integer>0</integer><key>value</key><integer>51</integer>"
				"<key>max</key><integer>127</integer></dict></dict>"
		, new BackLightPort()));

	return root;
}