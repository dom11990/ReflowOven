/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#ifndef _EXAMPLE_FILE_NAME_H    /* Guard against multiple inclusion */
#define _EXAMPLE_FILE_NAME_H


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */

/* TODO:  Include other files here if needed. */


/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif


    /* ************************************************************************** */
    /* ************************************************************************** */
    /* Section: Constants                                                         */
    /* ************************************************************************** */
    /* ************************************************************************** */

    /*  A brief description of a section can be given directly below the section
        banner.
     */


    /* ************************************************************************** */
    /** Descriptive Constant Name

      @Summary
        Brief one-line summary of the constant.
    
      @Description
        Full description, explaining the purpose and usage of the constant.
        <p>
        Additional description in consecutive paragraphs separated by HTML 
        paragraph breaks, as necessary.
        <p>
        Type "JavaDoc" in the "How Do I?" IDE toolbar for more information on tags.
    
      @Remarks
        Any additional remarks
     */

    // *****************************************************************************
    // Section: Data Types
    // *****************************************************************************
    // *****************************************************************************

    /*  A brief description of a section can be given directly below the section
        banner.
     */


    // *****************************************************************************

    /** Descriptive Data Type Name

      @Summary
        Brief one-line summary of the data type.
    
      @Description
        Full description, explaining the purpose and usage of the data type.
        <p>
        Additional description in consecutive paragraphs separated by HTML 
        paragraph breaks, as necessary.
        <p>
        Type "JavaDoc" in the "How Do I?" IDE toolbar for more information on tags.

      @Remarks
        Any additional remarks
        <p>
        Describe enumeration elements and structure and union members above each 
        element or member.
     */
    typedef struct _pid_settings {
        /* Describe structure member. */
        float pk;
        float ik;
        float dk;
        float last_error;
        float integral;
    } pid_t;
    
    typedef pid_t *pid_instance_t;


    // *****************************************************************************
    // *****************************************************************************
    // Section: Interface Functions
    // *****************************************************************************
    // *****************************************************************************

    /*  A brief description of a section can be given directly below the section
        banner.
     */

    // *****************************************************************************
    /**
     * Creates a new PID instance with the provided parameters
     * @param pk
     * @param ik
     * @param dk
     * @return 
     */
    pid_instance_t Create_PID(float pk, float ik, float dk);

    /**
     * Releases memory associated with the PID instance
     * @param pi
     */
    void PID_Destroy(pid_instance_t pi);

    /**
     * Calculates the result of the pid control and stores the current error for
     * the next call in the instance provided.
     * @param pi
     * @param variable
     * @param set_point
     * @return 
     */
    float PID_Compute(pid_instance_t pi, float variable, float set_point);

    /**
     * Reconfigures an existing instance with new parameters
     * @param pi pid instance
     * @param pk proportional constant
     * @param ik integral constant
     * @param dk derivative constant
     */
    void PID_Configure_Parameters(pid_instance_t pi, float pk, float ik, float dk);
    
    /**
     * Resets the internal state of the PID controller (integral and error = 0)
     * @param pi
     */
    void PID_Reset_State(pid_instance_t pi);
    
    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
