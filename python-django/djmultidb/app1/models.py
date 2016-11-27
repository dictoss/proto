from django.db import models


class Thing(models.Model):
    id = models.AutoField(primary_key=True)
    name = models.CharField(max_length=255)
    # it is False auto_now and auto_now_add, for replcation.
    # must set application create_at and update_at.
    create_at = models.DateTimeField(auto_now=False, auto_now_add=False)
    update_at = models.DateTimeField(auto_now=False, auto_now_add=False)
