from bluepy.btle import Peripheral, UUID
from bluepy.btle import Scanner, DefaultDelegate


class ScanDelegate(DefaultDelegate):
    def __init__(self):
        DefaultDelegate.__init__(self)

    def handleDiscovery(self, dev, isNewDev, isNewData):
        if isNewDev:
            print("Discovered device", dev.addr)
        elif isNewData:
            print("Received new data from", dev.addr)


class MyDelegate(DefaultDelegate):
    def __init__(self):
        DefaultDelegate.__init__(self)

    def handleNotification(self, cHandle, data):
        if cHandle == 19:
            ia = [
                int.from_bytes(data[i : i + 2], "little", signed="True")
                for i in range(0, len(data), 2)
            ]
            print(
                f"A notification was received: {ia[0]} {ia[1]} {ia[2]} from {cHandle}"
            )
        else:
            i = int.from_bytes(data, "big")
            print(f"A notification was received: {i} from {cHandle}")


scanner = Scanner().withDelegate(ScanDelegate())
devices = scanner.scan(10.0)
n = 0
addr = []

for dev in devices:
    print("%d: Device %s (%s), RSSI=%d dB" % (n, dev.addr, dev.addrType, dev.rssi))
    addr.append(dev.addr)
    n += 1

    for adtype, desc, value in dev.getScanData():
        print(" %s = %s" % (desc, value))

number = input("Enter your device number: ")
print("Device", number)
num = int(number)
print(addr[num])

print("Connecting...")
dev = Peripheral(addr[num], "random")
dev.setDelegate(MyDelegate())

print("Services...")
for svc in dev.services:
    print(str(svc))

try:
    hrmService = dev.getServiceByUUID(UUID(0x180D))
    for ch in hrmService.getCharacteristics():
        print(str(ch))
    hr = dev.getCharacteristics(uuid=UUID(0x2A37))[0]
    print(f"Heartrate cHandle: {hr.getHandle()}")
    cccd_descriptor = hr.getDescriptors(forUUID=0x2902)[0]
    new_cccd_value = bytearray([0x01, 0x00])
    cccd_descriptor.write(new_cccd_value)

    magnetoService = dev.getServiceByUUID(UUID(0x180B))
    for ch in magnetoService.getCharacteristics():
        print(str(ch))
    mg = dev.getCharacteristics(uuid=UUID(0x2A3D))[0]
    print(f"magneto cHandle: {mg.getHandle()}")
    cccd_descriptor = mg.getDescriptors(forUUID=0x2902)[0]
    new_cccd_value = bytearray([0x01, 0x00])
    cccd_descriptor.write(new_cccd_value)
    while True:
        if dev.waitForNotifications(1.0):
            continue
        print("waiting")
finally:
    dev.disconnect()

