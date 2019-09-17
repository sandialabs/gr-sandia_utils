#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Stream Gate Example
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
from gnuradio import qtgui
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import sandia_utils
import sip
import sys
from gnuradio import qtgui


class stream_gate_example(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "Stream Gate Example")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Stream Gate Example")
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

        self.settings = Qt.QSettings("GNU Radio", "stream_gate_example")
        self.restoreGeometry(self.settings.value("geometry").toByteArray())


        ##################################################
        # Variables
        ##################################################
        self.samp_rate = samp_rate = 30.72e6
        self.freq = freq = 915e6
        self.flow1 = flow1 = False
        self.flow0 = flow0 = False
        self.consume1 = consume1 = True
        self.consume0 = consume0 = True

        ##################################################
        # Blocks
        ##################################################
        _flow1_check_box = Qt.QCheckBox('Channel 1: Flow')
        self._flow1_choices = {True: True, False: False}
        self._flow1_choices_inv = dict((v,k) for k,v in self._flow1_choices.iteritems())
        self._flow1_callback = lambda i: Qt.QMetaObject.invokeMethod(_flow1_check_box, "setChecked", Qt.Q_ARG("bool", self._flow1_choices_inv[i]))
        self._flow1_callback(self.flow1)
        _flow1_check_box.stateChanged.connect(lambda i: self.set_flow1(self._flow1_choices[bool(i)]))
        self.top_grid_layout.addWidget(_flow1_check_box, 0, 1, 1, 1)
        for r in range(0, 1):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(1, 2):
            self.top_grid_layout.setColumnStretch(c, 1)
        _flow0_check_box = Qt.QCheckBox('Channel 0: Flow')
        self._flow0_choices = {True: True, False: False}
        self._flow0_choices_inv = dict((v,k) for k,v in self._flow0_choices.iteritems())
        self._flow0_callback = lambda i: Qt.QMetaObject.invokeMethod(_flow0_check_box, "setChecked", Qt.Q_ARG("bool", self._flow0_choices_inv[i]))
        self._flow0_callback(self.flow0)
        _flow0_check_box.stateChanged.connect(lambda i: self.set_flow0(self._flow0_choices[bool(i)]))
        self.top_grid_layout.addWidget(_flow0_check_box, 0, 0, 1, 1)
        for r in range(0, 1):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(0, 1):
            self.top_grid_layout.setColumnStretch(c, 1)
        _consume1_check_box = Qt.QCheckBox('Channel 1: Consume')
        self._consume1_choices = {True: True, False: False}
        self._consume1_choices_inv = dict((v,k) for k,v in self._consume1_choices.iteritems())
        self._consume1_callback = lambda i: Qt.QMetaObject.invokeMethod(_consume1_check_box, "setChecked", Qt.Q_ARG("bool", self._consume1_choices_inv[i]))
        self._consume1_callback(self.consume1)
        _consume1_check_box.stateChanged.connect(lambda i: self.set_consume1(self._consume1_choices[bool(i)]))
        self.top_grid_layout.addWidget(_consume1_check_box, 1, 1, 1, 1)
        for r in range(1, 2):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(1, 2):
            self.top_grid_layout.setColumnStretch(c, 1)
        _consume0_check_box = Qt.QCheckBox('Channel 0: Consume')
        self._consume0_choices = {True: True, False: False}
        self._consume0_choices_inv = dict((v,k) for k,v in self._consume0_choices.iteritems())
        self._consume0_callback = lambda i: Qt.QMetaObject.invokeMethod(_consume0_check_box, "setChecked", Qt.Q_ARG("bool", self._consume0_choices_inv[i]))
        self._consume0_callback(self.consume0)
        _consume0_check_box.stateChanged.connect(lambda i: self.set_consume0(self._consume0_choices[bool(i)]))
        self.top_grid_layout.addWidget(_consume0_check_box, 1, 0, 1, 1)
        for r in range(1, 2):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(0, 1):
            self.top_grid_layout.setColumnStretch(c, 1)
        self.sandia_utils_stream_gate_0_0 = sandia_utils.stream_gate_cc(flow1,consume1)
        self.sandia_utils_stream_gate_0 = sandia_utils.stream_gate_cc(flow0,consume0)
        self.qtgui_waterfall_sink_x_0_0 = qtgui.waterfall_sink_c(
        	1024, #size
        	firdes.WIN_BLACKMAN_hARRIS, #wintype
        	freq, #fc
        	samp_rate, #bw
        	"", #name
                1 #number of inputs
        )
        self.qtgui_waterfall_sink_x_0_0.set_update_time(0.01)
        self.qtgui_waterfall_sink_x_0_0.enable_grid(False)
        self.qtgui_waterfall_sink_x_0_0.enable_axis_labels(True)

        if not True:
          self.qtgui_waterfall_sink_x_0_0.disable_legend()

        if "complex" == "float" or "complex" == "msg_float":
          self.qtgui_waterfall_sink_x_0_0.set_plot_pos_half(not True)

        labels = ['', '', '', '', '',
                  '', '', '', '', '']
        colors = [0, 0, 0, 0, 0,
                  0, 0, 0, 0, 0]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
                  1.0, 1.0, 1.0, 1.0, 1.0]
        for i in xrange(1):
            if len(labels[i]) == 0:
                self.qtgui_waterfall_sink_x_0_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_waterfall_sink_x_0_0.set_line_label(i, labels[i])
            self.qtgui_waterfall_sink_x_0_0.set_color_map(i, colors[i])
            self.qtgui_waterfall_sink_x_0_0.set_line_alpha(i, alphas[i])

        self.qtgui_waterfall_sink_x_0_0.set_intensity_range(-140, 10)

        self._qtgui_waterfall_sink_x_0_0_win = sip.wrapinstance(self.qtgui_waterfall_sink_x_0_0.pyqwidget(), Qt.QWidget)
        self.top_grid_layout.addWidget(self._qtgui_waterfall_sink_x_0_0_win, 2, 1, 1, 1)
        for r in range(2, 3):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(1, 2):
            self.top_grid_layout.setColumnStretch(c, 1)
        self.qtgui_waterfall_sink_x_0 = qtgui.waterfall_sink_c(
        	1024, #size
        	firdes.WIN_BLACKMAN_hARRIS, #wintype
        	freq, #fc
        	samp_rate, #bw
        	"", #name
                1 #number of inputs
        )
        self.qtgui_waterfall_sink_x_0.set_update_time(0.01)
        self.qtgui_waterfall_sink_x_0.enable_grid(False)
        self.qtgui_waterfall_sink_x_0.enable_axis_labels(True)

        if not True:
          self.qtgui_waterfall_sink_x_0.disable_legend()

        if "complex" == "float" or "complex" == "msg_float":
          self.qtgui_waterfall_sink_x_0.set_plot_pos_half(not True)

        labels = ['', '', '', '', '',
                  '', '', '', '', '']
        colors = [0, 0, 0, 0, 0,
                  0, 0, 0, 0, 0]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
                  1.0, 1.0, 1.0, 1.0, 1.0]
        for i in xrange(1):
            if len(labels[i]) == 0:
                self.qtgui_waterfall_sink_x_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_waterfall_sink_x_0.set_line_label(i, labels[i])
            self.qtgui_waterfall_sink_x_0.set_color_map(i, colors[i])
            self.qtgui_waterfall_sink_x_0.set_line_alpha(i, alphas[i])

        self.qtgui_waterfall_sink_x_0.set_intensity_range(-140, 10)

        self._qtgui_waterfall_sink_x_0_win = sip.wrapinstance(self.qtgui_waterfall_sink_x_0.pyqwidget(), Qt.QWidget)
        self.top_grid_layout.addWidget(self._qtgui_waterfall_sink_x_0_win, 2, 0, 1, 1)
        for r in range(2, 3):
            self.top_grid_layout.setRowStretch(r, 1)
        for c in range(0, 1):
            self.top_grid_layout.setColumnStretch(c, 1)
        self.blocks_throttle_0 = blocks.throttle(gr.sizeof_gr_complex*1, samp_rate,True)
        self.analog_sig_source_x_0 = analog.sig_source_c(samp_rate, analog.GR_COS_WAVE, 1000, 1, 0)



        ##################################################
        # Connections
        ##################################################
        self.connect((self.analog_sig_source_x_0, 0), (self.blocks_throttle_0, 0))
        self.connect((self.blocks_throttle_0, 0), (self.sandia_utils_stream_gate_0, 0))
        self.connect((self.blocks_throttle_0, 0), (self.sandia_utils_stream_gate_0_0, 0))
        self.connect((self.sandia_utils_stream_gate_0, 0), (self.qtgui_waterfall_sink_x_0, 0))
        self.connect((self.sandia_utils_stream_gate_0_0, 0), (self.qtgui_waterfall_sink_x_0_0, 0))

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "stream_gate_example")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.qtgui_waterfall_sink_x_0_0.set_frequency_range(self.freq, self.samp_rate)
        self.qtgui_waterfall_sink_x_0.set_frequency_range(self.freq, self.samp_rate)
        self.blocks_throttle_0.set_sample_rate(self.samp_rate)
        self.analog_sig_source_x_0.set_sampling_freq(self.samp_rate)

    def get_freq(self):
        return self.freq

    def set_freq(self, freq):
        self.freq = freq
        self.qtgui_waterfall_sink_x_0_0.set_frequency_range(self.freq, self.samp_rate)
        self.qtgui_waterfall_sink_x_0.set_frequency_range(self.freq, self.samp_rate)

    def get_flow1(self):
        return self.flow1

    def set_flow1(self, flow1):
        self.flow1 = flow1
        self._flow1_callback(self.flow1)
        self.sandia_utils_stream_gate_0_0.set_flow_data(self.flow1)

    def get_flow0(self):
        return self.flow0

    def set_flow0(self, flow0):
        self.flow0 = flow0
        self._flow0_callback(self.flow0)
        self.sandia_utils_stream_gate_0.set_flow_data(self.flow0)

    def get_consume1(self):
        return self.consume1

    def set_consume1(self, consume1):
        self.consume1 = consume1
        self._consume1_callback(self.consume1)
        self.sandia_utils_stream_gate_0_0.set_consume_data(self.consume1)

    def get_consume0(self):
        return self.consume0

    def set_consume0(self, consume0):
        self.consume0 = consume0
        self._consume0_callback(self.consume0)
        self.sandia_utils_stream_gate_0.set_consume_data(self.consume0)


def main(top_block_cls=stream_gate_example, options=None):

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
