import datetime
import io

from pynequick import cli

def test__cli():

    fh = io.StringIO()
    cli.to_gim_file(cli.NequickCoeffs(236.831641, -0.39362878, 0.00402826613), datetime.datetime.now(), fh)

    doc = fh.getvalue()

    assert '# epoch: ' in doc
    assert '# longitude:' in doc
    assert '# latitude:' in doc

    assert len(doc.splitlines()) == 74
