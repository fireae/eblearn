# # copy scripts to bin
# ################################################################################
# SET(FILE_TO_BIN 
#   face/face_meta.conf 
#   face/face_dsprepare.sh 
#   face/face_mitcbcl_meta.conf 
#   face/face_mitcbcl_dsprepare.sh 
#   face/face_train.sh
#   text/text_meta.conf
#   text/text_train.sh
#   text/text_dsprepare.sh	
#   hand/hand_meta.conf
#   hand/hand_train.sh
#   hand/hand_dsprepare.sh
#   pedestrians/ped_meta.conf
#   pedestrians/ped_train.sh
#   pedestrians/ped_dsprepare.sh
# )

# FOREACH(NAME ${FILE_TO_BIN})
#   EXEC_PROGRAM("cp" ARGS "\"${CMAKE_CURRENT_SOURCE_DIR}/${NAME}\""
# 		  "\"${LIBRARY_OUTPUT_PATH}\"" OUTPUT_VARIABLE OUT)
#   MESSAGE(STATUS "Copied demos/obj/${NAME} to bin directory.")
# ENDFOREACH(NAME)

#ADD_SUBDIRECTORY(pascal)
#ADD_SUBDIRECTORY(window)

#ADD_SUBDIRECTORY(posture)
#ADD_SUBDIRECTORY(unsup_vision)
#ADD_SUBDIRECTORY(facenet)
#ADD_SUBDIRECTORY(sp_Doc_classifier)
