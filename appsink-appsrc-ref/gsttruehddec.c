/* GStreamer
 * Copyright (C) 2018 FIXME <fixme@example.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Suite 500,
 * Boston, MA 02110-1335, USA.
 */
/**
 * SECTION:element-gsttruehddec
 *
 * The truehddec element does FIXME stuff.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch-1.0 -v fakesrc ! truehddec ! FIXME ! fakesink
 * ]|
 * FIXME Describe what the pipeline does.
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include <gst/audio/gstaudiodecoder.h>
#include "gsttruehddec.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


GST_DEBUG_CATEGORY_STATIC (gst_truehddec_debug_category);
#define GST_CAT_DEFAULT gst_truehddec_debug_category

/* prototypes */


static void gst_truehddec_set_property (GObject * object,
		guint property_id, const GValue * value, GParamSpec * pspec);
static void gst_truehddec_get_property (GObject * object,
		guint property_id, GValue * value, GParamSpec * pspec);
static void gst_truehddec_dispose (GObject * object);
static void gst_truehddec_finalize (GObject * object);

static gboolean gst_truehddec_start (GstAudioDecoder * decoder);
static gboolean gst_truehddec_stop (GstAudioDecoder * decoder);
static gboolean gst_truehddec_set_format (GstAudioDecoder * decoder, GstCaps * caps);
static GstFlowReturn gst_truehddec_parse (GstAudioDecoder * decoder,
		GstAdapter * adapter, gint * offset, gint * length);
static GstFlowReturn gst_truehddec_handle_frame (GstAudioDecoder * decoder,
		GstBuffer * buffer);
static void gst_truehddec_flush (GstAudioDecoder * decoder, gboolean hard);
static GstFlowReturn gst_truehddec_pre_push (GstAudioDecoder * decoder,
		GstBuffer ** buffer);
static gboolean gst_truehddec_sink_event (GstAudioDecoder * decoder,
		GstEvent * event);
static gboolean gst_truehddec_src_event (GstAudioDecoder * decoder, GstEvent * event);
static gboolean gst_truehddec_open (GstAudioDecoder * decoder);
static gboolean gst_truehddec_close (GstAudioDecoder * decoder);
static gboolean gst_truehddec_negotiate (GstAudioDecoder * decoder);
static gboolean gst_truehddec_decide_allocation (GstAudioDecoder * decoder,
		GstQuery * query);
static gboolean gst_truehddec_propose_allocation (GstAudioDecoder * decoder,
		GstQuery * query);

enum
{
	PROP_0
};
int count = 0;

/* pad templates */

/* FIXME add/remove the formats that you want to support */
static GstStaticPadTemplate gst_truehddec_src_template =
GST_STATIC_PAD_TEMPLATE ("src",
		GST_PAD_SRC,
		GST_PAD_ALWAYS,
		GST_STATIC_CAPS ("audio/x-raw,format=S16LE,rate=[1,max],"
			"channels=[1,max],layout=interleaved")
		);

static GstStaticPadTemplate gst_truehddec_sink_template =
GST_STATIC_PAD_TEMPLATE ("sink",
		GST_PAD_SINK,
		GST_PAD_ALWAYS,
		GST_STATIC_CAPS ("Application/Unknown")
		);


/* class initialization */

G_DEFINE_TYPE_WITH_CODE (GstTruehddec, gst_truehddec, GST_TYPE_AUDIO_DECODER,
		GST_DEBUG_CATEGORY_INIT (gst_truehddec_debug_category, "truehddec", 0,
			"debug category for truehddec element"));

	static void
gst_truehddec_class_init (GstTruehddecClass * klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GstAudioDecoderClass *audio_decoder_class = GST_AUDIO_DECODER_CLASS (klass);

	/* Setting up pads and setting metadata should be moved to
	   base_class_init if you intend to subclass this class. */
	gst_element_class_add_static_pad_template (GST_ELEMENT_CLASS(klass),
			&gst_truehddec_src_template);
	gst_element_class_add_static_pad_template (GST_ELEMENT_CLASS(klass),
			&gst_truehddec_sink_template);

	gst_element_class_set_static_metadata (GST_ELEMENT_CLASS(klass),
			"FIXME Long name", "Generic", "FIXME Description",
			"FIXME <fixme@example.com>");

	gobject_class->set_property = gst_truehddec_set_property;
	gobject_class->get_property = gst_truehddec_get_property;
	gobject_class->dispose = gst_truehddec_dispose;
	gobject_class->finalize = gst_truehddec_finalize;
	audio_decoder_class->start = GST_DEBUG_FUNCPTR (gst_truehddec_start);
	audio_decoder_class->stop = GST_DEBUG_FUNCPTR (gst_truehddec_stop);
	//audio_decoder_class->set_format = GST_DEBUG_FUNCPTR (gst_truehddec_set_format);
	//audio_decoder_class->parse = GST_DEBUG_FUNCPTR (gst_truehddec_parse);
	audio_decoder_class->handle_frame = GST_DEBUG_FUNCPTR (gst_truehddec_handle_frame);
	//audio_decoder_class->flush = GST_DEBUG_FUNCPTR (gst_truehddec_flush);
	//audio_decoder_class->pre_push = GST_DEBUG_FUNCPTR (gst_truehddec_pre_push);
    audio_decoder_class->sink_event = GST_DEBUG_FUNCPTR (gst_truehddec_sink_event);
	//audio_decoder_class->src_event = GST_DEBUG_FUNCPTR (gst_truehddec_src_event);
	//audio_decoder_class->open = GST_DEBUG_FUNCPTR (gst_truehddec_open);
	//audio_decoder_class->close = GST_DEBUG_FUNCPTR (gst_truehddec_close);
	//audio_decoder_class->negotiate = GST_DEBUG_FUNCPTR (gst_truehddec_negotiate);
	//audio_decoder_class->decide_allocation = GST_DEBUG_FUNCPTR (gst_truehddec_decide_allocation);
	//audio_decoder_class->propose_allocation = GST_DEBUG_FUNCPTR (gst_truehddec_propose_allocation);
	fprintf(stderr, "Class init\n");
}

	static void
gst_truehddec_init (GstTruehddec *truehddec)
{
	gst_audio_decoder_set_use_default_pad_acceptcaps (GST_AUDIO_DECODER_CAST
			      (truehddec), TRUE);
	  GST_PAD_SET_ACCEPT_TEMPLATE (GST_AUDIO_DECODER_SINK_PAD (truehddec));
}

void
gst_truehddec_set_property (GObject * object, guint property_id,
    const GValue * value, GParamSpec * pspec)
{
  GstTruehddec *truehddec = GST_TRUEHDDEC (object);

  GST_DEBUG_OBJECT (truehddec, "set_property");

  switch (property_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

void
gst_truehddec_get_property (GObject * object, guint property_id,
    GValue * value, GParamSpec * pspec)
{
  GstTruehddec *truehddec = GST_TRUEHDDEC (object);

  GST_DEBUG_OBJECT (truehddec, "get_property");

  switch (property_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

void
gst_truehddec_dispose (GObject * object)
{
  GstTruehddec *truehddec = GST_TRUEHDDEC (object);

  GST_DEBUG_OBJECT (truehddec, "dispose");

  /* clean up as possible.  may be called multiple times */

  G_OBJECT_CLASS (gst_truehddec_parent_class)->dispose (object);
}

void
gst_truehddec_finalize (GObject * object)
{
  GstTruehddec *truehddec = GST_TRUEHDDEC (object);

  GST_DEBUG_OBJECT (truehddec, "finalize");

  /* clean up object here */

  G_OBJECT_CLASS (gst_truehddec_parent_class)->finalize (object);
}

static gboolean
gst_truehddec_start (GstAudioDecoder * decoder)
{
  GstTruehddec *truehddec = GST_TRUEHDDEC (decoder);

  GST_DEBUG_OBJECT (truehddec, "start");

  return TRUE;
}

static gboolean
gst_truehddec_stop (GstAudioDecoder * decoder)
{
  GstTruehddec *truehddec = GST_TRUEHDDEC (decoder);

  GST_DEBUG_OBJECT (truehddec, "stop");

  return TRUE;
}

static gboolean
gst_truehddec_set_format (GstAudioDecoder * decoder, GstCaps * caps)
{
  GstTruehddec *truehddec = GST_TRUEHDDEC (decoder);

  GST_DEBUG_OBJECT (truehddec, "set_format");
  
  fprintf(stderr, "SET FORMAT\n");

  return TRUE;
}

static GstFlowReturn
gst_truehddec_parse (GstAudioDecoder * decoder, GstAdapter * adapter, gint * offset,
    gint * length)
{
  GstTruehddec *truehddec = GST_TRUEHDDEC (decoder);

  GST_DEBUG_OBJECT (truehddec, "parse");

  return GST_FLOW_OK;
}

static GstFlowReturn
gst_truehddec_handle_frame (GstAudioDecoder * decoder, GstBuffer * buffer)
{
  GstTruehddec *truehddec = GST_TRUEHDDEC (decoder);
  GstAudioChannelPosition position[6] = {GST_AUDIO_CHANNEL_POSITION_NONE};
  GstAudioInfo ainfo;
  GstMapInfo imap;
  GstMapInfo omap;
  gsize input_size;
  guchar *input_data;
  gsize output_size;

  /* no fancy draining */
  if (G_UNLIKELY (!buffer))
	    return GST_FLOW_OK;

  gst_buffer_ref(buffer);
  gst_buffer_map (buffer, &imap, GST_MAP_READ);
  input_data = imap.data;
  input_size = imap.size;
  fprintf(stderr, "Handle Frame -> Input Buffer size = %d\n", input_size);
  gst_buffer_unmap (buffer, &imap);

  gst_audio_info_init(&ainfo);
  gst_audio_info_set_format(&ainfo, GST_AUDIO_FORMAT_S16, 48000, 1, position);
  gst_audio_decoder_set_output_format(decoder, &ainfo);

  GST_DEBUG_OBJECT (truehddec, "handle_frame");

  if (!count)
  {
	count++;
  	fprintf(stderr, "sent NULL\n");
  	gst_buffer_unref(buffer);
  	return gst_audio_decoder_finish_frame(decoder, NULL, 1);
  }

  GstBuffer *buf1 = gst_audio_decoder_allocate_output_buffer (decoder, input_size);
  gst_buffer_map (buffer, &imap, GST_MAP_READ);
  gst_buffer_map (buf1, &omap, GST_MAP_READWRITE);
  memcpy(omap.data, imap.data, imap.size);
  gst_buffer_unmap (buffer, &imap);
  gst_buffer_unmap (buf1, &omap);
  gst_audio_decoder_finish_frame(decoder, buf1, 1);
  fprintf(stderr, "sent frame1\n");

  GstBuffer *buf2 = gst_audio_decoder_allocate_output_buffer (decoder, input_size);
  gst_buffer_map (buffer, &imap, GST_MAP_READ);
  gst_buffer_map (buf2, &omap, GST_MAP_READWRITE);
  memcpy(omap.data, imap.data, imap.size);
  gst_buffer_unmap (buffer, &imap);
  gst_buffer_unmap (buf2, &omap);
  gst_audio_decoder_finish_frame(decoder, buf2, 1);
  fprintf(stderr, "sent frame2\n");

  gst_buffer_unref(buffer);
  return GST_FLOW_OK;
}

static void
gst_truehddec_flush (GstAudioDecoder * decoder, gboolean hard)
{
  GstTruehddec *truehddec = GST_TRUEHDDEC (decoder);

  GST_DEBUG_OBJECT (truehddec, "flush");

}

static GstFlowReturn
gst_truehddec_pre_push (GstAudioDecoder * decoder, GstBuffer ** buffer)
{
  GstTruehddec *truehddec = GST_TRUEHDDEC (decoder);

  GST_DEBUG_OBJECT (truehddec, "pre_push");

  return GST_FLOW_OK;
}

static gboolean
gst_truehddec_sink_event (GstAudioDecoder * decoder, GstEvent * event)
{
  gboolean ret;
  GstTruehddec *truehddec = GST_TRUEHDDEC (decoder);

  GST_DEBUG_OBJECT (truehddec, "sink_event");
  fprintf(stderr, "gst_truehddec_sink_event\n");

switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_SEGMENT:
      /* maybe save and/or update the current segment (e.g. for output
       * clipping) or convert the event into one in a different format
       * (e.g. BYTES to TIME) or drop it and set a flag to send a segment
       * event in a different format later */
      ret = gst_pad_push_event (GST_AUDIO_DECODER_SRC_PAD(decoder), event);
      break;
    case GST_EVENT_EOS:
      /* end-of-stream, we should close down all stream leftovers here */
      ret = gst_pad_push_event (GST_AUDIO_DECODER_SRC_PAD(decoder), event);
      break;
    case GST_EVENT_FLUSH_STOP:
      ret = gst_pad_push_event (GST_AUDIO_DECODER_SRC_PAD(decoder), event);
      break;
    default:
      ret = gst_pad_event_default (GST_AUDIO_DECODER_SINK_PAD(decoder), GST_OBJECT_CAST(decoder), event);
      break;
  }

  return ret;
}

static gboolean
gst_truehddec_src_event (GstAudioDecoder * decoder, GstEvent * event)
{
  GstTruehddec *truehddec = GST_TRUEHDDEC (decoder);

  GST_DEBUG_OBJECT (truehddec, "src_event");

  return TRUE;
}

static gboolean
gst_truehddec_open (GstAudioDecoder * decoder)
{
  GstTruehddec *truehddec = GST_TRUEHDDEC (decoder);

  GST_DEBUG_OBJECT (truehddec, "open");

  return TRUE;
}

static gboolean
gst_truehddec_close (GstAudioDecoder * decoder)
{
  GstTruehddec *truehddec = GST_TRUEHDDEC (decoder);

  GST_DEBUG_OBJECT (truehddec, "close");

  return TRUE;
}

static gboolean
gst_truehddec_negotiate (GstAudioDecoder * decoder)
{
  GstTruehddec *truehddec = GST_TRUEHDDEC (decoder);

  GST_DEBUG_OBJECT (truehddec, "negotiate");

  return TRUE;
}

static gboolean
gst_truehddec_decide_allocation (GstAudioDecoder * decoder, GstQuery * query)
{
  GstTruehddec *truehddec = GST_TRUEHDDEC (decoder);

  GST_DEBUG_OBJECT (truehddec, "decide_allocation");

  return TRUE;
}

static gboolean
gst_truehddec_propose_allocation (GstAudioDecoder * decoder, GstQuery * query)
{
  GstTruehddec *truehddec = GST_TRUEHDDEC (decoder);

  GST_DEBUG_OBJECT (truehddec, "propose_allocation");

  return TRUE;
}

static gboolean
plugin_init (GstPlugin * plugin)
{

  /* FIXME Remember to set the rank if it's an element that is meant
     to be autoplugged by decodebin. */
  return gst_element_register (plugin, "truehddec", GST_RANK_NONE,
      GST_TYPE_TRUEHDDEC);
}

/* FIXME: these are normally defined by the GStreamer build system.
   If you are creating an element to be included in gst-plugins-*,
   remove these, as they're always defined.  Otherwise, edit as
   appropriate for your external plugin package. */
#ifndef VERSION
#define VERSION "0.0.FIXME"
#endif
#ifndef PACKAGE
#define PACKAGE "FIXME_package"
#endif
#ifndef PACKAGE_NAME
#define PACKAGE_NAME "FIXME_package_name"
#endif
#ifndef GST_PACKAGE_ORIGIN
#define GST_PACKAGE_ORIGIN "http://FIXME.org/"
#endif

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    truehddec,
    "FIXME plugin description",
    plugin_init, VERSION, "LGPL", PACKAGE_NAME, GST_PACKAGE_ORIGIN)

