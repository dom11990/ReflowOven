/* ************************************************************************** */

#include "pid.h"
#include "system_definitions.h"

/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.c

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */

/* TODO:  Include other files here if needed. */


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */

/*  A brief description of a section can be given directly below the section
    banner.
 */

/* ************************************************************************** */
/** Descriptive Data Item Name

  @Summary
    Brief one-line summary of the data item.
    
  @Description
    Full description, explaining the purpose and usage of data item.
    <p>
    Additional description in consecutive paragraphs separated by HTML 
    paragraph breaks, as necessary.
    <p>
    Type "JavaDoc" in the "How Do I?" IDE toolbar for more information on tags.
    
  @Remarks
    Any additional remarks
 */


/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */

/*  A brief description of a section can be given directly below the section
    banner.
 */

/* ************************************************************************** */


pid_instance_t * Create_PID(float pk, float ik, float dk) {
    pid_instance_t * pi = pvPortMalloc(sizeof (pid_instance_t));

    //fail to allocated?
    if (!pi)
        return 0;

    //allocation successful, assign the parameters
    pi->pk = pk;
    pi->ik = ik;
    pi->dk = dk;
    pi->last_error = 0;
    pi->integral = 0;
    return pi;
}

void Destroy_PID(pid_instance_t* pi) {
    if (pi)
        vPortFree(pi);
}

float Compute(pid_instance_t * pi, float variable, float set_point) {
    float result;
    float derivative;
    float error = set_point - variable;
    
    pi->integral += error;
    derivative = error - pi->last_error;
    result = error*pi->pk + pi->integral*pi->ik + derivative*pi->dk;
    //update the error
    pi->last_error = error;
    return result;
}

/* *****************************************************************************
 End of File
 */
