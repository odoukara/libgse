/****************************************************************************/
/**
 *   @file          header_fields.c
 *
 *          Project:     GSE LIBRARY
 *
 *          Company:     THALES ALENIA SPACE
 *
 *          Module name: COMMON
 *
 *   @brief         Function related to header fields
 *
 *   @author        Julien BERNARD / Viveris Technologies
 *
 */
/****************************************************************************/

#include "header_fields.h"

#include <arpa/inet.h>

#include "header.h"

/****************************************************************************
 *
 *   PUBLIC FUNCTIONS
 *
 ****************************************************************************/

gse_status_t gse_get_start_indicator(unsigned char *packet,
                                     uint8_t *start_indicator)
{
  gse_status_t status = GSE_STATUS_OK;
  gse_header_t *header;

  if(packet == NULL)
  {
    status = GSE_STATUS_NULL_PTR;
    goto error;
  }

  header = (gse_header_t *)packet;
  *start_indicator = header->s;

error:
  return status;
}

gse_status_t gse_get_end_indicator(unsigned char *packet,
                                   uint8_t *end_indicator)
{
  gse_status_t status = GSE_STATUS_OK;
  gse_header_t *header;

  if(packet == NULL)
  {
    status = GSE_STATUS_NULL_PTR;
    goto error;
  }

  header = (gse_header_t *)packet;
  *end_indicator = header->e;

error:
  return status;
}

gse_status_t gse_get_label_type(unsigned char *packet, uint8_t *label_type)
{
  gse_status_t status = GSE_STATUS_OK;
  gse_header_t *header;

  if(packet == NULL)
  {
    status = GSE_STATUS_NULL_PTR;
    goto error;
  }

  header = (gse_header_t *)packet;
  *label_type = header->lt;

error:
  return status;
}

gse_status_t gse_get_gse_length(unsigned char *packet, uint16_t *gse_length)
{
  gse_status_t status = GSE_STATUS_OK;
  gse_header_t *header;

  if(packet == NULL)
  {
    status = GSE_STATUS_NULL_PTR;
    goto error;
  }

  header = (gse_header_t *)packet;
  *gse_length = ((uint16_t)header->gse_length_hi << 8) | header->gse_length_lo;

error:
  return status;
}

gse_status_t gse_get_frag_id(unsigned char *packet, uint8_t *frag_id)
{
  gse_status_t status = GSE_STATUS_OK;
  gse_header_t *header;

  if(packet == NULL)
  {
    status = GSE_STATUS_NULL_PTR;
    goto error;
  }

  header = (gse_header_t *)packet;
  // Test if the GSE packet contains a fragment of PDU and not a complete PDU
  if(header->s == 1 && header->e == 1)
  {
    status = GSE_STATUS_FIELD_ABSENT;
    goto error;
  }
  // Set the Frag Id according to payload type
  if(header->s == 0)
  {
    *frag_id = header->subs_frag_s.frag_id;
  }
  else
  {
    *frag_id = header->first_frag_s.frag_id;
  }

error:
  return status;
}

gse_status_t gse_get_total_length(unsigned char *packet, uint16_t *total_length)
{
  gse_status_t status = GSE_STATUS_OK;
  gse_header_t *header;

  if(packet == NULL)
  {
    status = GSE_STATUS_NULL_PTR;
    goto error;
  }

  header = (gse_header_t *)packet;
  // Test if the GSE packet contains the good fragment type
  if(header->s != 1 || header->e != 0)
  {
    status = GSE_STATUS_FIELD_ABSENT;
    goto error;
  }
  *total_length = ntohs(header->first_frag_s.total_length);

error:
  return status;
}

gse_status_t gse_get_protocol_type(unsigned char *packet,
                                   uint16_t *protocol_type)
{
  gse_status_t status = GSE_STATUS_OK;
  gse_header_t *header;

  if(packet == NULL)
  {
    status = GSE_STATUS_NULL_PTR;
    goto error;
  }

  header = (gse_header_t *)packet;
  // Test if the GSE packet contains the good payload type
  if(header->s != 1)
  {
    status = GSE_STATUS_FIELD_ABSENT;
    goto error;
  }
  // Set the Protocol Type according to payload type
  if(header->e == 0)
  {
    *protocol_type = ntohs(header->first_frag_s.protocol_type);
  }
  else
  {
    *protocol_type = ntohs(header->complete_s.protocol_type);
  }

error:
  return status;
}

gse_status_t gse_get_label(unsigned char *packet, uint8_t label[6])
{
  gse_status_t status = GSE_STATUS_OK;
  gse_header_t *header;

  if(packet == NULL)
  {
    status = GSE_STATUS_NULL_PTR;
    goto error;
  }

  header = (gse_header_t *)packet;
  // Test if the GSE packet contains the good payload type
  if(header->s != 1)
  {
    status = GSE_STATUS_FIELD_ABSENT;
    goto error;
  }
  // Set the Label according to payload type
  if(header->e == 0)
  {
    memcpy(label, &header->first_frag_s.label,
           gse_get_label_length(header->lt));
  }
  else
  {
    memcpy(label, &header->complete_s.label,
           gse_get_label_length(header->lt));
  }

error:
  return status;
}
