import datetime
import tempfile

from pynequick import cli

def test__cli():

    with tempfile.NamedTemporaryFile(mode='wt') as fh:
        gim_handler = cli.GimFileHandler(fh)
        cli.to_gim(cli.NequickCoeffs(236.831641, -0.39362878, 0.00402826613), datetime.datetime.now(), gim_handler = gim_handler)

        fh.seek(0)

        with open(fh.name, mode='rt') as fin:
            doc = fin.read()

            assert '# epoch: ' in doc
            assert '# longitude:' in doc
            assert '# latitude:' in doc

            assert len(doc.splitlines()) == 74
