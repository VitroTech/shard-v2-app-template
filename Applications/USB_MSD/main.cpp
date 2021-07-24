#include <mbed.h>
#include <rtc_api_hal.h>

#include <shard-sdk.h>
#include <vitroio-sdk/communication/can_layer.h>
#include <global_consts.h>

#include "USBMSD.h"
#include "FATFileSystem.h"

using namespace vitroio::sdk;

#define MAIN_MODULE_NAME "MAIN"
#define MAIN_INFO(format, ...) VITROIO_DEBUG_INFO(MAIN_MODULE_NAME, format, ##__VA_ARGS__);
#define MAIN_WARNING(format, ...) VITROIO_DEBUG_WARNING(MAIN_MODULE_NAME, format, ##__VA_ARGS__);
#define MAIN_ERROR(format, ...) VITROIO_DEBUG_ERROR(MAIN_MODULE_NAME, format, ##__VA_ARGS__);

#define SERIAL_BAUDRATE 115200

#define LBYTE_MASK 0xFF
#define UNIQUE_ID_REG ((uint32_t*) 0x1FFF7590)

#define START_FRAME_PARAMETER_ID 0x7FF
#define TIMESTAMP_REQUEST_PARAMETER_ID 0x40

#define DHT_DATA_LINE_PIN       PC_10
#define LIGHT_SENS_DATA_LINE    PC_11


#define DEFAULT_BLOCK_SIZE  512
#define BLOCK_COUNT 128
#define HEAP_BLOCK_DEVICE_SIZE (BLOCK_COUNT * DEFAULT_BLOCK_SIZE)
#define MAX_CON_ATTEMPTS 3

// Watchdog
Watchdog wdt;
Ticker wdtKicker;

// Peripherals
Serial pc(UART_DEBUG_TX, UART_DEBUG_RX, SERIAL_BAUDRATE);
DigitalOut statusLed(VITROIO_TEMPLATE_STATUS_LED_PIN);

// Event queues and threads
Thread normalPriorityThread(osPriorityNormal, 0x1800);
EventQueue normalPriorityEventQueue(32*EVENTS_EVENT_SIZE);
Thread highPriorityThread(osPriorityHigh, 0x1800);
EventQueue highPriorityEventQueue(32*EVENTS_EVENT_SIZE);

ShardEdge shardEdgeConfig(false, false, false, false, 16, 16, 16, 16);

static const uint32_t bank = 0;

// vitroio-sdk objects
Canbus canbus(CAN_RX, CAN_TX, &highPriorityEventQueue);

NodeController node(
    &canbus,
    FirmwareId((uint16_t)VITRIOIO_TEMPLATE_FIRMWARE_ID),
    Version(
        VITROIO_TEMPLATE_VERSION_MAJOR,
        VITROIO_TEMPLATE_VERSION_MINOR,
        VITROIO_TEMPLATE_VERSION_PATCH,
        VITROIO_TEMPLATE_VERSION_RC),
    &highPriorityEventQueue
);

/*******************************************************************************

        USB

*******************************************************************************/
// Threads
Thread USB_runner;
Thread FS_runner;

// Semaphores
// File system initialized
Semaphore FS_ok(0,1);
// USB disconnected
Semaphore MediaRemoved(1, 1);
// USB process requirement
Semaphore USBProc(0);

FATFileSystem heap_fs("heap_fs");
HeapBlockDevice bd(HEAP_BLOCK_DEVICE_SIZE, DEFAULT_BLOCK_SIZE);

IoTBlock* dataBlock;
// USB connection
DigitalOut usbbb(PC_6);
InterruptIn usbVbus(PA_9);
// Detection of VBUS presence
bool isConnected = DigitalIn(PA_9).read();
// Mounted to host device flag.
bool conn = false;

// Rising interrupt callback, usb connected
void riseVbus(){
    isConnected = true;
}

// Falling interrupt callback, usb disconnected
void fallVbus(){
    isConnected = false;
}

// USB process request callback
void usb_callback (void){
    // Inform mian thread to process usb.
    USBProc.release();
}

// Mount file system
void FS_mount (){
    usbbb = 0;
    MAIN_INFO("FS:\t Mounting block Device\r\n");
    int err = heap_fs.mount(&bd);

    if (err) {
        MAIN_INFO("FS:\t %s filesystem mount failed\n\ttry to reformat device... \r\n", heap_fs.getName());
        err = heap_fs.reformat(&bd);
    }

    // If still error, then report failure
    if (err) {
        MAIN_INFO("FS:\t Error: Unable to format/mount the device.\r\n");
        while (1){ThisThread::sleep_for(osWaitForever);}
    }
}

// Unmount file system
void FS_unmount (){
    MAIN_INFO("FS:\t Unmounting block device.\r\n");
    heap_fs.unmount();
}

uint8_t payloadFromFile[MAX_PD_SIZE - 16] = {0};

// File system thread
void FS_thread() {
    MediaRemoved.wait();
    MAIN_INFO("FS_thread:\t File system thread starting.\r\n");
    bd.init();
    FATFileSystem::format(&bd);
    FS_mount();
    FILE* fd = fopen("/heap_fs/test.txt", "w");
    fclose(fd);
    FS_unmount();
    FS_ok.release(); // give back semaphore
    MAIN_INFO("FS_thread:\t File system thread running.\r\n");

    while(1){
        // Take semaphore
        MediaRemoved.wait();
        FS_mount();
        MAIN_INFO("FS_thread:\t Media removed.\r\nFS_thread:\t Opening test.txt\r\n");
        fd = fopen("/heap_fs/test.txt", "r");
        if(fd){
            MAIN_INFO("FS_thread:\t Dumping file to screen.\r\n");
            MAIN_INFO("**********************************************\r\n\r\n");

            uint8_t par = 0;
            while (!feof(fd)){
                int size = fread(&payloadFromFile[0], 1, MAX_PD_SIZE - 16, fd);
                //fwrite(&buff[0], 1, size, stdout);
                dataBlock->make(payloadFromFile, size, 0x132);
                dataBlock->print();
                memset(payloadFromFile, 0, MAX_PD_SIZE - 16);
                par++;
                ThisThread::sleep_for(5);
            }
            MAIN_INFO("EOF.\r\n");
            MAIN_INFO("**********************************************\r\n");
            NVIC_SystemReset();
        }else{
            MAIN_INFO("FS_thread:\t No file found.\r\n");
        }
        FATFileSystem::format(&bd);
        fd = fopen("/heap_fs/test.txt", "w");
        fclose(fd);
        FS_unmount();
    }
}

// USB Mass Storage Device
USBMSD* usb;//(&bd, false, 1155, 22314);

// USB thread
void USB_thread() {
    uint32_t connAttempts = 0;
    bool failed = false;
    MAIN_INFO("USB_thread:\t USB system thread starting.");
    // Wait for FS to init.
    usbVbus.rise(&riseVbus);
    usbVbus.fall(&fallVbus);
    FS_ok.wait();
    // Attach usb process callback
    MAIN_INFO("USB_thread:\t Creating usb object.");
    usb = new USBMSD(&bd, false, 1155, 22314);
    usb->attach(usb_callback);
    // Try to mount to host.
    //FS_unmount();
    MAIN_INFO("USB_thread:\t Mounting to USB host...");
    if(isConnected) conn = usb->connect();
    else conn = false;
    MAIN_INFO("USB_thread:\t USB system thread running.\r\n");
    while(1){
        // If cable connected, but device is unmounted to host
        if(isConnected && !conn){
            if(connAttempts < MAX_CON_ATTEMPTS){
                failed = false;
                MAIN_INFO("USB_thread:\t Mounting to USB host. %d\r\n", connAttempts);
                conn = usb->connect();
                if(!conn){
                    connAttempts++;
                }else{
                    MAIN_INFO("USB_Thread:\t Mounted.\r\n");
                }
            }else{
                failed = true;
                usb->disconnect();
                MAIN_INFO("USB_thread:\t Could not mount to USB host.\r\n");
                connAttempts = 0;
                conn = true;
            }
        // If cable disconnected, unmount from host.
        }else if(!isConnected && conn){
            MAIN_INFO("USB_thread:\t Unmounted from USB host.\r\n");
            usb->disconnect();
            conn = false;
            connAttempts = 0;
            // If connection was successfull, inform FS thread about disconnection.
            if(!failed){
                MediaRemoved.release();
            }
        }
        ThisThread::sleep_for(5);
    }
}
/******************************************************************************/

// global variable for timestamps from Gateway
volatile uint32_t timestampGlobal = 0;

void frameReceivedCallback(const CanbusFrame& frame)
{
    if (frame.parameterId() == TIMESTAMP_REQUEST_PARAMETER_ID)
    {
        timestampGlobal = *((uint32_t*)frame.data());
    }
}

int main()
{
    MAIN_INFO("Application started; (id: %d) v%d.%d.%d.%d; vitroio-sdk v%s",
        VITRIOIO_TEMPLATE_FIRMWARE_ID,
        VITROIO_TEMPLATE_VERSION_MAJOR, VITROIO_TEMPLATE_VERSION_MINOR, VITROIO_TEMPLATE_VERSION_PATCH, VITROIO_TEMPLATE_VERSION_RC,
        VITROIO_SDK_VERSION);

    wdtKicker.attach(callback(&wdt, &Watchdog::Service), 2.0);

    highPriorityThread.start(callback(&highPriorityEventQueue, &EventQueue::dispatch_forever));
    normalPriorityThread.start(callback(&normalPriorityEventQueue, &EventQueue::dispatch_forever));

    if(!canbus.isValid() || !node.isValid() /*|| !measurementApi.isValid()*/){
        MAIN_ERROR("Failed to initialize vitroio-sdk objects");
        //NVIC_SystemReset();
    }

    if (shardEdgeConfig.configureShardEdge() != 0){
        MAIN_ERROR("Failed to configure Shard Edge");
        //NVIC_SystemReset();
    }

    int err = node.initCommunication();
    if(err != VITROIO_ERR_SUCCESS){
        MAIN_ERROR("Failed to initialize communication");
        //NVIC_SystemReset();
    }

    MAIN_INFO("mbed-os-rev: %d.%d.%d", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);

    Can_layer can_layer = Can_layer(&canbus, node.nodeId());
    dataBlock = new IoTBlock(&can_layer);
    //ThisThread::set_priority(osPriorityHigh);
    FS_runner.set_priority(osPriorityLow);
    FS_runner.start(FS_thread);
    USB_runner.start(USB_thread);

    while(1) {
        USBProc.wait();
        usb->process();
    }

    return 0;
}
