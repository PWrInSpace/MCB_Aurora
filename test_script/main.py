import serial
import time
import logging
import threading as th
import os.path
import numpy as np

class LoRaTest():
    TEST_PACKET_NB = 200
    TEST_OUTPUT_DIR = "outputs/"

    def __init__(self, antenna_name, distance) -> None:
        self.ser = serial.Serial("/dev/ttyACM0", baudrate=115200, timeout=1)
        self.received_package = 0
        self.transmited_package = 0
        self.unable_to_decode = 0
        self.lock = th.Lock()
        self.file_name = antenna_name + "_" + distance + ".txt"
        self._file_exist(self.TEST_OUTPUT_DIR + self.file_name)
        logging.info("Log file -> %s", self.TEST_OUTPUT_DIR + self.file_name)
        self.file = open(self.TEST_OUTPUT_DIR + self.file_name, "w")

    def _file_exist(self, file_path):
        if os.path.isfile(file_path) is True:
            raise Exception("File exists")

    def on_timer(self):
        self.ser.write("R4A;DUPA;GS;NOW".encode())
        self.transmited_package += 1
        if self.transmited_package < self.TEST_PACKET_NB:
            th.Timer(0.5, self.on_timer).start()
        else:
            self.lock.release()

    def start_test(self):
        self._reset_dev()
        self.lock.acquire()
        th.Timer(0.5, self.on_timer).start()
        logging.info("Starting test")
        self._test_loop()
        self._print_test_stats()

    def _reset_dev(self):
        self.ser.write("RESET;GS;NOW".encode())
        logging.info("Reseting device ....")
        time.sleep(1)
        self.ser.flush()

    def _print_test_stats(self):
        test_data = np.loadtxt(self.TEST_OUTPUT_DIR + self.file_name, delimiter=";", dtype=str)
        rssi = np.array([int(z) for z in test_data[:,3]])
        snr = np.array([float(z) for z in test_data[:,4]])
        logging.info("Received %d message", self.received_package)
        logging.info("Lost %d message", self.TEST_PACKET_NB - self.received_package)
        logging.info("Precent of received %f", self.received_package/float(self.TEST_PACKET_NB) * 100)
        logging.info("Unable to decode %d", self.unable_to_decode)
        logging.info("RSSI:\tAverage %f\tMAX %d\tMin %d", np.average(rssi), max(rssi), min(rssi))
        logging.info("SNR:\tAverage %f\tMAX %f\tMin %f", np.average(snr), max(snr), min(snr))
        with open(self.TEST_OUTPUT_DIR + "stats_" + self.file_name, "w") as stats_file:
            stats_file.write(f"Received {self.received_package} message\n")
            stats_file.write(f"Lost {self.TEST_PACKET_NB - self.received_package} message\n")
            stats_file.write(f"Precent of received {self.received_package/float(self.TEST_PACKET_NB) * self.TEST_PACKET_NB}\n")
            stats_file.write(f"RSSI:\tAverage {np.average(rssi)}\tMAX {max(rssi)}\tMin {min(rssi)}\n")
            stats_file.write(f"SNR:\tAverage {np.average(snr)}\tMAX {max(snr)}\tMin {min(snr)}\n")
            stats_file.write(f"Unable to decode {self.unable_to_decode}\n")
    def _test_loop(self):
        while self.lock.locked():
            try:
                line = self.ser.readline().decode()
                logging.info("Receive -> %s...", line[:25])
                self.file.write(line)
                self.received_package += 1
            except Exception as err:
                self.unable_to_decode += 1
                logging.error("Unable to decode message")

        self.file.close()


if __name__ == "__main__":
    logging.getLogger().setLevel(logging.INFO)
    antenna = input("Insert antenna name: ")
    distance = input("Insert distance: ")
    test = LoRaTest(antenna, distance)
    test.start_test()