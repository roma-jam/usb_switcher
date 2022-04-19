from pywinusb import hid
   
filter = hid.HidDeviceFilter(vendor_id = 0x24DC, product_id = 0x0FFE)
hid_device = filter.get_devices()
##type (hid_device)
##print (hid_device)
device = hid_device[0]
device.open()
print(hid_device)


target_usage = hid.get_full_usage_id(0x0002, 0xFA1D)
##device.set_raw_data_handler(sample_handler)
print(target_usage)


report = device.find_output_reports()

print(report)
print(report[0])

buffer = [0x00]*64
buffer[0] = 63
#buffer[0] = 0x02

# Switch ON       
#buffer[1] = 0x01 

# Switch OFF
#buffer[1] = 0x00

print(len(buffer))

report[0].set_raw_data(buffer)
report[0].send()
