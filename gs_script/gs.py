import struct
import logging
import time
import argparse
from serial import Serial
from datetime import datetime
from dataclasses import dataclass

# litle_endian, float, float, float, uchar, uchar, short int, short int
#               lat    long   alt    rcnt   mrcnt  rssi      snr
message_format = "<fffBBhh"
message_prefix = "B"
uplink_period_s = 60

@dataclass
class Downlink:
    laititude: float
    longtitude: float
    gps_altitude: float
    received_counter: int
    missreceived_counter: int
    mcb_rssi: int
    mcv_snr: int

    @classmethod
    def getArgsNamesCSV(cls) -> str:
        message = ""
        for key in cls.__dict__['__annotations__']:
            message += f"{key};"

        return message[:-1] + '\n'

    def getArgsValuesCSV(self) -> str:
        message = ""
        for _, val in self.__dict__.items():
            message += f"{val};"

        return message[:-1] + '\n'

def parse_message(message: bytes) -> Downlink:
    if bytes([message[0]]) != b'B':
        logging.error("Invalid prefix :C")
        return None
    
    message_data = struct.unpack(message_format, message[1:])
    
    return Downlink(*message_data)


def script_args():
    parser = argparse.ArgumentParser("GS message decoding")
    parser.add_argument("port")
    parser.add_argument("-b", "--baudrate", help="baudrate", default=115200)
    logging.basicConfig(level=logging.INFO)

    return parser.parse_args()


def append_to_file(file_path: str, text: str) -> None:
    with open(file_path, 'a') as file:
        file.write(text)


def get_next_uplink_time() -> int:
    return time.time() + uplink_period_s


def generate_file_path() -> str:
    now = datetime.now()
    date_time = now.strftime("%d%m%Y_%H:%M:%S")
    return f"baloon_{date_time}.csv"


def add_checksum(message: bytes) -> int:
    checksum = 0
    for i in message:
        checksum += int(i)

    checksum = checksum % 255

    return message + checksum.to_bytes()

if __name__ == "__main__":
    args = script_args()
    ser = Serial(args.port, args.baudrate, timeout=0.05)

    file_path = generate_file_path()
    append_to_file(file_path, Downlink.getArgsNamesCSV()[:-1] + ";nb_of_uplinks\n")

    next_uplink_time = get_next_uplink_time()
    nb_of_uplinks = 0

    while True:
        message = ser.read(20)
        if message:
            logging.info(f"Received downlink: {message}")
            decoded_data = parse_message(message)
            logging.info(f"Decoded message {decoded_data}")
    
            if decoded_data:
                logging.info(f"Saving to file: {file_path}")
                append_to_file(file_path, decoded_data.getArgsValuesCSV()[-1] + f";{nb_of_uplinks}\n")
                logging.info("Done")
            
        if time.time() > next_uplink_time:
            next_uplink_time = get_next_uplink_time()
            uplink_message = add_checksum(b"Hello balloon")
            nb_of_uplinks += 1
            print(f"Uplink nb {nb_of_uplinks}: {uplink_message}")

            ser.write(uplink_message)