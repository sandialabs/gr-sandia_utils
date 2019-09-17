#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: File Sink Example
# GNU Radio version: 3.7.13.5
##################################################

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print "Warning: failed to XInitThreads()"

from PyQt4 import Qt
from gnuradio import analog
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import sandia_utils
import sys
import timing_utils
from gnuradio import qtgui


class file_sink_example(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "File Sink Example")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("File Sink Example")
        qtgui.util.check_set_qss()
        try:
            self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except:
            pass
        self.top_scroll_layout = Qt.QVBoxLayout()
        self.setLayout(self.top_scroll_layout)
        self.top_scroll = Qt.QScrollArea()
        self.top_scroll.setFrameStyle(Qt.QFrame.NoFrame)
        self.top_scroll_layout.addWidget(self.top_scroll)
        self.top_scroll.setWidgetResizable(True)
        self.top_widget = Qt.QWidget()
        self.top_scroll.setWidget(self.top_widget)
        self.top_layout = Qt.QVBoxLayout(self.top_widget)
        self.top_grid_layout = Qt.QGridLayout()
        self.top_layout.addLayout(self.top_grid_layout)

        self.settings = Qt.QSettings("GNU Radio", "file_sink_example")
        self.restoreGeometry(self.settings.value("geometry").toByteArray())


        ##################################################
        # Variables
        ##################################################
        self.tag_now = tag_now = 0
        self.samp_rate = samp_rate = 32000
        self.record = record = True

        ##################################################
        # Blocks
        ##################################################
        _record_check_box = Qt.QCheckBox('Record?')
        self._record_choices = {True: True, False: False}
        self._record_choices_inv = dict((v,k) for k,v in self._record_choices.iteritems())
        self._record_callback = lambda i: Qt.QMetaObject.invokeMethod(_record_check_box, "setChecked", Qt.Q_ARG("bool", self._record_choices_inv[i]))
        self._record_callback(self.record)
        _record_check_box.stateChanged.connect(lambda i: self.set_record(self._record_choices[bool(i)]))
        self.top_grid_layout.addWidget(_record_check_box)
        self.timing_utils_add_usrp_tags_c_0 = timing_utils.add_usrp_tags_c(915e6, int(samp_rate), 123456789, .5)
        _tag_now_push_button = Qt.QPushButton('Tag Now')
        self._tag_now_choices = {'Pressed': 1, 'Released': 0}
        _tag_now_push_button.pressed.connect(lambda: self.set_tag_now(self._tag_now_choices['Pressed']))
        _tag_now_push_button.released.connect(lambda: self.set_tag_now(self._tag_now_choices['Released']))
        self.top_grid_layout.addWidget(_tag_now_push_button)
        self.sandia_utils_file_sink_0 = sandia_utils.file_sink("complex", gr.sizeof_gr_complex*1, 'raw', sandia_utils.MANUAL, 10*int(samp_rate), int(samp_rate), '/mnt/ramdisk', "%03fd.fc32")
        self.sandia_utils_file_sink_0.set_recording(record)
        self.sandia_utils_file_sink_0.set_gen_new_folder(False)
        self.sandia_utils_file_sink_0.set_second_align(False)
        self.blocks_throttle_0 = blocks.throttle(gr.sizeof_gr_complex*1, samp_rate,True)
        self.blocks_tag_debug_0 = blocks.tag_debug(gr.sizeof_gr_complex*1, '', ""); self.blocks_tag_debug_0.set_display(True)
        self.blocks_message_debug_0 = blocks.message_debug()
        self.analog_sig_source_x_0 = analog.sig_source_c(samp_rate, analog.GR_COS_WAVE, 1000, 1, 0)



        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.sandia_utils_file_sink_0, 'msg'), (self.blocks_message_debug_0, 'print'))
        self.connect((self.analog_sig_source_x_0, 0), (self.timing_utils_add_usrp_tags_c_0, 0))
        self.connect((self.blocks_throttle_0, 0), (self.sandia_utils_file_sink_0, 0))
        self.connect((self.timing_utils_add_usrp_tags_c_0, 0), (self.blocks_tag_debug_0, 0))
        self.connect((self.timing_utils_add_usrp_tags_c_0, 0), (self.blocks_throttle_0, 0))

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "file_sink_example")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()

    def get_tag_now(self):
        return self.tag_now

    def set_tag_now(self, tag_now):
        self.tag_now = tag_now
        self.timing_utils_add_usrp_tags_c_0.tag_now(self.tag_now)

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.sandia_utils_file_sink_0.set_nsamples(10*int(self.samp_rate))
        self.blocks_throttle_0.set_sample_rate(self.samp_rate)
        self.analog_sig_source_x_0.set_sampling_freq(self.samp_rate)

    def get_record(self):
        return self.record

    def set_record(self, record):
        self.record = record
        self._record_callback(self.record)
        self.sandia_utils_file_sink_0.set_recording(self.record)


def main(top_block_cls=file_sink_example, options=None):

    from distutils.version import StrictVersion
    if StrictVersion(Qt.qVersion()) >= StrictVersion("4.5.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls()
    tb.start()
    tb.show()

    def quitting():
        tb.stop()
        tb.wait()
    qapp.connect(qapp, Qt.SIGNAL("aboutToQuit()"), quitting)
    qapp.exec_()


if __name__ == '__main__':
    main()
