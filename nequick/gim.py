from abc import ABC, abstractmethod
from dataclasses import dataclass
import datetime
import io
from typing import List

import numpy as np

@dataclass
class Gim():
    epoch: datetime.datetime
    longitudes: List[float]
    latitudes: List[float]
    vtec_values: List[List[float]]  # Grid of VTEC values n_latitudes (rows) x n_longitudes (columns)


class GimHandler(ABC):

    @abstractmethod
    def process(self, gim: Gim):
        """
        Process a GIM file
        """
        pass

class GimFileHandler(GimHandler):

    def __init__(self, file: io.TextIOWrapper):
        self.file = file

    def process(self, gim: Gim):
        """
        Process a GIM file
        """

        lon_start = gim.longitudes[0]
        lon_end = gim.longitudes[-1]
        lon_n = len(gim.longitudes)

        lat_start = gim.latitudes[0]
        lat_end = gim.latitudes[-1]
        lat_n = len(gim.latitudes)

        header = f'# epoch: {gim.epoch.isoformat()}\n' + \
                 f'# longitude:: start: {lon_start:7.2f} end: {lon_end:7.2f} n: {lon_n:3d}\n' + \
                 f'# latitude:: start: {lat_start:7.2f} end: {lat_end:7.2f} n: {lat_n:3d}\n'

        self.file.write(header)

        np.savetxt(self.file, gim.vtec_values, "%7.3f")
