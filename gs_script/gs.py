import struct
import logging
import argparse
from serial import Serial
from datetime import datetime
from dataclasses import dataclass

# litle_endian, float, float, float, uchar, uchar, short int, short int
#               lat    long   alt    rcnt   mrcnt  rssi      snr
message_format = "<fffBBhh"
message_prefix = "B"

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


def generate_file_path() -> str:
    now = datetime.now()
    date_time = now.strftime("%m:%d:%Y_%H:%M:%S")
    return f"baloon_{date_time}.csv"


if __name__ == "__main__":
    args = script_args()
    ser = Serial(args.port, args.baudrate, timeout=0.05)

    file_path = generate_file_path()
    append_to_file(file_path, Downlink.getArgsNamesCSV())

    while True:
        message = ser.read(20)
        if message:
            logging.info(f"Received downlink: {message}")
            decoded_data = parse_message(message)
            logging.info(f"Decoded message {decoded_data}")
    
            if decoded_data:
                logging.info(f"Saving to file: {file_path}")
                append_to_file(file_path, decoded_data.getArgsValuesCSV())
                logging.info("Done")
            