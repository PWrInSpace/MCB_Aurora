import serial
import time
import logging
import threading as th

class LoRaTest():
    TEST_PACKET_NB = 100

    def __init__(self, file_name) -> None:
        self.ser = serial.Serial("/dev/ttyACM0", baudrate=115200)
        self.received_package = 0
        self.transmited_package = 0
        self.lock = th.Lock()
        self.file_name = file_name
        logging.info("Log file -> %s", file_name)
        self.file = open(file_name, "w")

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
        logging.info("Received %d message", self.received_package)
        logging.info("Lost %d message", self.TEST_PACKET_NB - self.received_package)
        logging.info("Precent of received %f", self.received_package/float(self.TEST_PACKET_NB) * self.TEST_PACKET_NB)
        with open("stats_" + self.file_name, "w") as stats_file:
            stats_file.write(f"Received {self.received_package} message\n")
            stats_file.write(f"Lost {self.TEST_PACKET_NB - self.received_package} message\n")
            stats_file.write(f"Precent of received {self.received_package/float(self.TEST_PACKET_NB) * self.TEST_PACKET_NB}\n")


    def _test_loop(self):
        while self.lock.locked():
            try:
                line = self.ser.readline().decode()
                logging.info("Receive -> %s...", line[:25])
                self.file.write(line)
                self.received_package += 1
            except Exception as err:
                logging.error("Unable to decode message")


if __name__ == "__main__":
    logging.getLogger().setLevel(logging.INFO)
    file_name = input("Insert file name: ")
    if file_name.endswith(".txt") or file_name.endswith(".csv"):
        test = LoRaTest(file_name)
        test.start_test()
    else:
        logging.error("Invalid file name %s", file_name)