import datetime
import subprocess
import tempfile

from nequick import cli, to_gim

def test__cli():

    proc = subprocess.run(['nequick', '--coefficients', '236.831641', '-0.39362878', '0.00402826613'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    assert proc.returncode == 0

    doc = proc.stdout.decode()

    assert '# epoch: ' in doc
    assert '# longitude:' in doc
    assert '# latitude:' in doc

    assert len(doc.splitlines()) == 74

