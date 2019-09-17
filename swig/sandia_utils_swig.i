/* -*- c++ -*- */

#define SANDIA_UTILS_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "sandia_utils_swig_doc.i"

%{
#include "sandia_utils/file_sink.h"
#include "sandia_utils/burst_power_detector.h"
#include "sandia_utils/tag_debug_file.h"
#include "sandia_utils/invert_tune.h"
#include "sandia_utils/block_buffer.h"
#include "sandia_utils/message_vector_file_sink.h"
#include "sandia_utils/message_vector_raster_file_sink.h"
#include "sandia_utils/stream_gate_cc.h"
#include "sandia_utils/stream_gate_ff.h"
#include "sandia_utils/stream_gate_ss.h"
#include "sandia_utils/stream_gate_bb.h"
#include "sandia_utils/file_source.h"
#include "sandia_utils/interleaved_short_to_complex.h"
#include "sandia_utils/complex_to_interleaved_short.h"
%}


%include "sandia_utils/file_sink.h"
GR_SWIG_BLOCK_MAGIC2(sandia_utils, file_sink);
%include "sandia_utils/burst_power_detector.h"
GR_SWIG_BLOCK_MAGIC2(sandia_utils, burst_power_detector);
%include "sandia_utils/tag_debug_file.h"
GR_SWIG_BLOCK_MAGIC2(sandia_utils, tag_debug_file);
%include "sandia_utils/invert_tune.h"
GR_SWIG_BLOCK_MAGIC2(sandia_utils, invert_tune);
%include "sandia_utils/block_buffer.h"
GR_SWIG_BLOCK_MAGIC2(sandia_utils, block_buffer);
%include "sandia_utils/message_vector_file_sink.h"
GR_SWIG_BLOCK_MAGIC2(sandia_utils, message_vector_file_sink);
%include "sandia_utils/message_vector_raster_file_sink.h"
GR_SWIG_BLOCK_MAGIC2(sandia_utils, message_vector_raster_file_sink);
%include "sandia_utils/stream_gate_cc.h"
GR_SWIG_BLOCK_MAGIC2(sandia_utils, stream_gate_cc);
%include "sandia_utils/stream_gate_ff.h"
GR_SWIG_BLOCK_MAGIC2(sandia_utils, stream_gate_ff);
%include "sandia_utils/stream_gate_ss.h"
GR_SWIG_BLOCK_MAGIC2(sandia_utils, stream_gate_ss);
%include "sandia_utils/stream_gate_bb.h"
GR_SWIG_BLOCK_MAGIC2(sandia_utils, stream_gate_bb);
%include "sandia_utils/file_source.h"
GR_SWIG_BLOCK_MAGIC2(sandia_utils, file_source);
%include "sandia_utils/interleaved_short_to_complex.h"
GR_SWIG_BLOCK_MAGIC2(sandia_utils, interleaved_short_to_complex);
%include "sandia_utils/complex_to_interleaved_short.h"
GR_SWIG_BLOCK_MAGIC2(sandia_utils, complex_to_interleaved_short);
